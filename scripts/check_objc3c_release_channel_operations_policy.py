#!/usr/bin/env python3
"""Validate the checked-in objc3c release-channel operations policy."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any, Mapping, Sequence

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "release_channel_operations_policy.json"
SCHEMA_PATH = ROOT / "schemas" / "objc3c-release-channel-operations-policy-v1.schema.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-channel-operations-policy-summary.json"

POLICY_CONTRACT_ID = "objc3c.release.channel.operations.policy.v1"
SCHEMA_ID = "https://objc3c.dev/schemas/objc3c-release-channel-operations-policy-v1.schema.json"
SUMMARY_CONTRACT_ID = "objc3c.release.channel.operations.policy.summary.v1"
REQUIRED_CHANNELS = ("stable", "nightly")
REQUIRED_RELEASABLE_EVIDENCE = (
    "release-foundation",
    "update-manifest",
    "rollback-proof",
    "signed-artifacts",
)
REQUIRED_FORBIDDEN_CLAIMS = (
    "hosted update service",
    "background auto-update service",
    "package registry launch",
    "public production release without gate evidence",
)
GENERATED_OUTPUT_PREFIXES = ("tmp/", "artifacts/")
ROLLBACK_COMMAND = "npm run objc3c -- validate-packaging-channels-end-to-end"
STABLE_REQUIRED_GATES = (
    "validate-release-foundation",
    "validate-release-candidate-conformance",
    "validate-release-operations-end-to-end",
    "validate-distribution-credibility-end-to-end",
)
NIGHTLY_REQUIRED_GATES = (
    "test-nightly",
    "validate-release-foundation",
    "validate-release-operations",
)


class ReleaseChannelPolicyError(RuntimeError):
    pass


def _fail(message: str) -> ReleaseChannelPolicyError:
    return ReleaseChannelPolicyError(message)


def require_mapping(payload: Mapping[str, Any], field_name: str) -> Mapping[str, Any]:
    value = payload.get(field_name)
    if not isinstance(value, Mapping):
        raise _fail(f"{field_name} must be an object")
    return value


def require_list(payload: Mapping[str, Any], field_name: str) -> list[Any]:
    value = payload.get(field_name)
    if not isinstance(value, list) or not value:
        raise _fail(f"{field_name} must be a non-empty list")
    return value


def require_string_list(payload: Mapping[str, Any], field_name: str) -> list[str]:
    values = require_list(payload, field_name)
    if not all(isinstance(value, str) and value for value in values):
        raise _fail(f"{field_name} must contain only non-empty strings")
    return list(values)


def require_bool(payload: Mapping[str, Any], field_name: str) -> bool:
    value = payload.get(field_name)
    if not isinstance(value, bool):
        raise _fail(f"{field_name} must be boolean")
    return value


def schema_contract_const(schema: Mapping[str, Any]) -> object:
    properties = schema.get("properties")
    if not isinstance(properties, Mapping):
        return None
    contract = properties.get("contract_id")
    if not isinstance(contract, Mapping):
        return None
    return contract.get("const")


def normalize_repo_path(relative_path: str, *, field_name: str) -> str:
    if not isinstance(relative_path, str) or not relative_path:
        raise _fail(f"{field_name} must be a non-empty repo-relative path")
    normalized = relative_path.replace("\\", "/")
    if normalized != relative_path:
        raise _fail(f"{field_name} must use slash paths: {relative_path}")
    path = Path(normalized)
    if path.is_absolute() or ".." in path.parts:
        raise _fail(f"{field_name} must be a repo-relative path: {relative_path}")
    return normalized


def is_generated_output_path(relative_path: str) -> bool:
    return relative_path.startswith(GENERATED_OUTPUT_PREFIXES)


def validate_schema(schema: Mapping[str, Any]) -> None:
    if schema.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
        raise _fail("schema must use draft 2020-12")
    if schema.get("$id") != SCHEMA_ID:
        raise _fail("schema id drifted")
    if schema_contract_const(schema) != POLICY_CONTRACT_ID:
        raise _fail("schema contract_id const drifted")
    required = schema.get("required")
    if not isinstance(required, list):
        raise _fail("schema required list missing")
    for field_name in (
        "required_channels",
        "update_policy",
        "rollback_policy",
        "channels",
        "fail_closed_rules",
    ):
        if field_name not in required:
            raise _fail(f"schema does not require {field_name}")


def ensure_repo_file(relative_path: str, *, field_name: str) -> None:
    normalized = normalize_repo_path(relative_path, field_name=field_name)
    if is_generated_output_path(normalized):
        raise _fail(f"{field_name} must not use generated output as source truth: {normalized}")
    target = ROOT / normalized
    if not target.is_file():
        raise _fail(f"{field_name} references missing file {normalized}")


def ensure_generated_evidence_path(relative_path: str, *, field_name: str) -> str:
    normalized = normalize_repo_path(relative_path, field_name=field_name)
    if not is_generated_output_path(normalized):
        raise _fail(f"{field_name} must be generated evidence, not source truth: {normalized}")
    return normalized


def validate_checked_source_list(
    payload: Mapping[str, Any],
    field_name: str,
    *,
    owner: str,
) -> list[str]:
    values = require_string_list(payload, field_name)
    for index, value in enumerate(values):
        ensure_repo_file(value, field_name=f"{owner}.{field_name}[{index}]")
    return values


def validate_generated_evidence_list(
    payload: Mapping[str, Any],
    field_name: str,
    *,
    owner: str,
) -> list[str]:
    values = require_string_list(payload, field_name)
    for index, value in enumerate(values):
        ensure_generated_evidence_path(value, field_name=f"{owner}.{field_name}[{index}]")
    return values


def validate_policy_surfaces(policy: Mapping[str, Any]) -> str:
    surfaces = require_mapping(policy, "policy_surfaces")
    expected_paths = {
        "source_policy": repo_rel(POLICY_PATH),
        "schema": repo_rel(SCHEMA_PATH),
        "validator": "scripts/check_objc3c_release_channel_operations_policy.py",
        "runbook": "docs/runbooks/objc3c_release_channel_operations.md",
    }
    for field_name, expected_path in expected_paths.items():
        value = surfaces.get(field_name)
        if value != expected_path:
            raise _fail(f"policy_surfaces.{field_name} drifted from {expected_path}")
        ensure_repo_file(expected_path, field_name=f"policy_surfaces.{field_name}")

    upstream_paths = require_string_list(surfaces, "upstream_checked_manifests")
    for upstream_path in upstream_paths:
        ensure_repo_file(upstream_path, field_name="policy_surfaces.upstream_checked_manifests")
    return str(surfaces["runbook"])


def validate_release_evidence_policy(policy: Mapping[str, Any]) -> list[str]:
    evidence_policy = require_mapping(policy, "release_evidence_policy")
    source_roots = require_string_list(evidence_policy, "source_truth_roots")
    for source_root in source_roots:
        normalized = normalize_repo_path(source_root, field_name="release_evidence_policy.source_truth_roots")
        if is_generated_output_path(normalized):
            raise _fail("release evidence source truth roots must not include generated output roots")

    requirements = require_list(evidence_policy, "required_for_releasable")
    class_ids: list[str] = []
    for requirement in requirements:
        if not isinstance(requirement, Mapping):
            raise _fail("required_for_releasable entries must be objects")
        class_id = requirement.get("class_id")
        if not isinstance(class_id, str) or not class_id:
            raise _fail("required_for_releasable entry missing class_id")
        if requirement.get("evidence_kind") != "generated-artifact":
            raise _fail(f"{class_id} releasable evidence must be generated-artifact")
        validate_generated_evidence_list(
            requirement,
            "required_paths",
            owner=f"release_evidence_policy.required_for_releasable.{class_id}",
        )
        commands = require_string_list(requirement, "required_commands")
        for command in commands:
            if not command.startswith("npm run objc3c -- "):
                raise _fail(f"{class_id} releasable evidence command is not public: {command}")
        class_ids.append(class_id)

    missing = sorted(set(REQUIRED_RELEASABLE_EVIDENCE) - set(class_ids))
    if missing:
        raise _fail("release evidence policy missing required classes: " + ", ".join(missing))
    return class_ids


def validate_update_policy(policy: Mapping[str, Any]) -> None:
    update_policy = require_mapping(policy, "update_policy")
    if update_policy.get("missing_channel_behavior") != "fail-closed":
        raise _fail("update policy must fail closed on missing channels")
    if update_policy.get("manifest_contract") != "objc3c.release.operations.update-manifest.v1":
        raise _fail("update policy manifest contract drifted")
    blocked = set(require_string_list(update_policy, "blocked_update_mechanics"))
    for required in ("hosted background updater", "package registry upgrade semantics", "silent cross-major upgrade"):
        if required not in blocked:
            raise _fail(f"update policy must block {required}")


def validate_rollback_policy(policy: Mapping[str, Any]) -> set[str]:
    rollback_policy = require_mapping(policy, "rollback_policy")
    if rollback_policy.get("missing_rollback_behavior") != "fail-closed":
        raise _fail("rollback policy must fail closed when rollback proof is missing")
    if rollback_policy.get("rollback_drill_command") != ROLLBACK_COMMAND:
        raise _fail("rollback drill command drifted")
    primitives = set(require_string_list(rollback_policy, "required_primitives"))
    missing = sorted({"local-installer", "offline-bundle"} - primitives)
    if missing:
        raise _fail("rollback policy missing primitives: " + ", ".join(missing))
    metadata = set(require_string_list(rollback_policy, "metadata_required"))
    for field_name in ("rollback_channel", "operator_command", "evidence", "blocks_publication_on_failure"):
        if field_name not in metadata:
            raise _fail(f"rollback metadata must require {field_name}")
    require_string_list(rollback_policy, "proof_required")
    return primitives


def channels_by_id(policy: Mapping[str, Any]) -> dict[str, Mapping[str, Any]]:
    channels = require_list(policy, "channels")
    result: dict[str, Mapping[str, Any]] = {}
    for channel in channels:
        if not isinstance(channel, Mapping):
            raise _fail("channels entries must be objects")
        channel_id = channel.get("channel_id")
        if not isinstance(channel_id, str) or not channel_id:
            raise _fail("channel missing channel_id")
        if channel_id in result:
            raise _fail(f"duplicate channel {channel_id}")
        result[channel_id] = channel
    return result


def validate_channel_releasability(
    channel_id: str,
    channel: Mapping[str, Any],
    required_evidence_classes: set[str],
) -> None:
    evidence = require_mapping(channel, "evidence")
    releasability = require_mapping(channel, "releasability")
    state = releasability.get("state")
    may_call_releasable = require_bool(releasability, "may_call_releasable")
    evidence_state = evidence.get("evidence_state")
    channel_required = set(require_string_list(evidence, "required_evidence_classes"))
    missing = sorted(required_evidence_classes - channel_required)
    if missing:
        raise _fail(f"{channel_id} channel missing releasable evidence classes: " + ", ".join(missing))
    if releasability.get("missing_evidence_behavior") != "fail-closed":
        raise _fail(f"{channel_id} channel must fail closed when releasable evidence is missing")
    if state == "releasable" or may_call_releasable:
        if evidence_state != "verified":
            raise _fail(f"{channel_id} channel cannot be releasable without verified evidence")
        verified_evidence = require_string_list(evidence, "verified_evidence")
        missing_verified = sorted(required_evidence_classes - set(verified_evidence))
        if missing_verified:
            raise _fail(f"{channel_id} channel verified evidence missing: " + ", ".join(missing_verified))


def validate_channel(
    channel_id: str,
    channel: Mapping[str, Any],
    required_evidence_classes: set[str],
    rollback_primitives: set[str],
) -> list[str]:
    if channel.get("operation_class") != channel_id:
        raise _fail(f"{channel_id} operation_class must match channel_id")

    publication = require_mapping(channel, "publication_mechanics")
    gates = require_string_list(publication, "gate_actions")
    if publication.get("notes_source_mode") != "source-derived":
        raise _fail(f"{channel_id} release notes must be source-derived")
    release_note_sources = validate_checked_source_list(
        publication,
        "release_notes_sources",
        owner=f"{channel_id}.publication_mechanics",
    )

    update = require_mapping(channel, "update_mechanics")
    if update.get("update_manifest_channel") != channel_id:
        raise _fail(f"{channel_id} update manifest channel drifted")
    if update.get("missing_manifest_behavior") != "fail-closed":
        raise _fail(f"{channel_id} update mechanics must fail closed on missing manifest")
    require_string_list(update, "permitted_targets")

    rollback = require_mapping(channel, "rollback_mechanics")
    primitive = rollback.get("primitive")
    if primitive not in rollback_primitives:
        raise _fail(f"{channel_id} rollback primitive {primitive!r} is not in rollback policy")
    if rollback.get("command") != ROLLBACK_COMMAND:
        raise _fail(f"{channel_id} rollback command drifted")
    if rollback.get("blocks_publication_on_failure") is not True:
        raise _fail(f"{channel_id} rollback must block publication on failure")
    require_string_list(rollback, "proof_required")

    claim_boundaries = require_mapping(channel, "claim_boundaries")
    if claim_boundaries.get("unproven_distribution_claims_not_made") is not True:
        raise _fail(f"{channel_id} must explicitly reject unproven distribution claims")
    require_string_list(claim_boundaries, "forbidden_claims")

    evidence = require_mapping(channel, "evidence")
    source_truth = validate_checked_source_list(
        evidence,
        "source_truth",
        owner=f"{channel_id}.evidence",
    )
    generated_evidence = validate_generated_evidence_list(
        evidence,
        "generated_evidence_artifacts",
        owner=f"{channel_id}.evidence",
    )
    if not set(release_note_sources).issubset(set(source_truth)):
        missing = sorted(set(release_note_sources) - set(source_truth))
        raise _fail(
            f"{channel_id} release-note sources must be listed as channel source truth: "
            + ", ".join(missing)
        )
    overlap = sorted(set(source_truth) & set(generated_evidence))
    if overlap:
        raise _fail(
            f"{channel_id} source truth overlaps generated evidence: "
            + ", ".join(overlap)
        )

    validate_channel_releasability(channel_id, channel, required_evidence_classes)
    return gates


def validate_required_channels(policy: Mapping[str, Any]) -> dict[str, Mapping[str, Any]]:
    required_channels = require_string_list(policy, "required_channels")
    channel_order = require_string_list(policy, "channel_order")
    if tuple(required_channels) != REQUIRED_CHANNELS:
        raise _fail("required_channels must be exactly stable, nightly")
    if tuple(channel_order) != REQUIRED_CHANNELS:
        raise _fail("channel_order must be exactly stable, nightly")

    channels = channels_by_id(policy)
    missing = [channel_id for channel_id in REQUIRED_CHANNELS if channel_id not in channels]
    if missing:
        raise _fail("missing required channels: " + ", ".join(missing))
    extra = sorted(set(channels) - set(REQUIRED_CHANNELS))
    if extra:
        raise _fail("unexpected channels in release-channel policy: " + ", ".join(extra))
    return channels


def validate_forbidden_claims(policy: Mapping[str, Any]) -> None:
    forbidden_claims = set(require_string_list(policy, "forbidden_distribution_claims"))
    missing = sorted(set(REQUIRED_FORBIDDEN_CLAIMS) - forbidden_claims)
    if missing:
        raise _fail("forbidden distribution claims missing: " + ", ".join(missing))

    rules = require_list(policy, "fail_closed_rules")
    for rule in rules:
        if not isinstance(rule, Mapping):
            raise _fail("fail_closed_rules entries must be objects")
        if rule.get("blocks_publication") is not True:
            raise _fail("all fail_closed_rules must block publication")


def validate_stable_nightly_split(channels: Mapping[str, Mapping[str, Any]]) -> tuple[list[str], list[str]]:
    stable = channels["stable"]
    nightly = channels["nightly"]
    stable_gates = validate_channel(
        "stable",
        stable,
        set(REQUIRED_RELEASABLE_EVIDENCE),
        {"local-installer", "offline-bundle"},
    )
    nightly_gates = validate_channel(
        "nightly",
        nightly,
        set(REQUIRED_RELEASABLE_EVIDENCE),
        {"local-installer", "offline-bundle"},
    )
    for required_gate in STABLE_REQUIRED_GATES:
        if required_gate not in stable_gates:
            raise _fail(f"stable channel gate missing {required_gate}")
    for required_gate in NIGHTLY_REQUIRED_GATES:
        if required_gate not in nightly_gates:
            raise _fail(f"nightly channel gate missing {required_gate}")
    if set(stable_gates) == set(nightly_gates):
        raise _fail("stable and nightly gates must remain distinct")

    stable_claims = require_mapping(stable, "claim_boundaries")
    nightly_claims = require_mapping(nightly, "claim_boundaries")
    if stable_claims.get("stable_support_claim_allowed") is not True:
        raise _fail("stable channel must be the only channel allowed to carry stable support after evidence")
    if nightly_claims.get("stable_support_claim_allowed") is not False:
        raise _fail("nightly channel must not be allowed to carry stable support")
    if nightly.get("support_claim_scope") != "nightly-evidence-only":
        raise _fail("nightly support scope must remain evidence-only")
    nightly_forbidden = set(require_string_list(nightly_claims, "forbidden_claims"))
    if "stable support" not in nightly_forbidden or "warning-free nightly promotion" not in nightly_forbidden:
        raise _fail("nightly forbidden claims must reject stable support and warning-free promotion")
    return stable_gates, nightly_gates


def validate_policy(policy: Mapping[str, Any], schema: Mapping[str, Any]) -> dict[str, Any]:
    validate_schema(schema)
    if policy.get("contract_id") != POLICY_CONTRACT_ID:
        raise _fail("policy contract_id drifted")
    if policy.get("schema_version") != 1:
        raise _fail("policy schema_version drifted")
    if policy.get("source_authority") != "checked-in-release-channel-operations-policy":
        raise _fail("policy source_authority drifted")
    if "source truth" not in str(policy.get("generated_artifact_role", "")):
        raise _fail("policy must distinguish generated evidence from source truth")

    runbook = validate_policy_surfaces(policy)
    evidence_class_ids = validate_release_evidence_policy(policy)
    validate_update_policy(policy)
    rollback_primitives = validate_rollback_policy(policy)
    validate_forbidden_claims(policy)
    channels = validate_required_channels(policy)
    stable_gates, nightly_gates = validate_stable_nightly_split(channels)

    blocked_channels = [
        channel_id
        for channel_id in REQUIRED_CHANNELS
        if require_mapping(channels[channel_id], "releasability").get("state")
        == "blocked-until-verified-evidence"
    ]
    releasable_channels = [
        channel_id
        for channel_id in REQUIRED_CHANNELS
        if require_mapping(channels[channel_id], "releasability").get("state") == "releasable"
    ]

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "policy": repo_rel(POLICY_PATH),
        "schema": repo_rel(SCHEMA_PATH),
        "runbook": runbook,
        "required_channels": list(REQUIRED_CHANNELS),
        "channel_order": list(REQUIRED_CHANNELS),
        "releasable_channels": releasable_channels,
        "blocked_until_verified_evidence_channels": blocked_channels,
        "required_releasable_evidence": evidence_class_ids,
        "rollback_primitives": sorted(rollback_primitives),
        "stable_gate_actions": stable_gates,
        "nightly_gate_actions": nightly_gates,
        "forbidden_distribution_claims": require_string_list(policy, "forbidden_distribution_claims"),
        "source_truth_path_count": sum(
            len(require_string_list(channels[channel_id]["evidence"], "source_truth"))
            for channel_id in REQUIRED_CHANNELS
        ),
        "generated_evidence_artifact_count": sum(
            len(require_string_list(channels[channel_id]["evidence"], "generated_evidence_artifacts"))
            for channel_id in REQUIRED_CHANNELS
        ),
    }


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--policy", type=Path, default=POLICY_PATH)
    parser.add_argument("--schema", type=Path, default=SCHEMA_PATH)
    parser.add_argument("--summary", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(sys.argv[1:] if argv is None else argv))
    policy = load_json(args.policy)
    schema = load_json(args.schema)
    summary = validate_policy(policy, schema)
    args.summary.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(args.summary, summary)
    print(f"summary_path: {repo_rel(args.summary)}")
    print("objc3c-release-channel-operations-policy: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
