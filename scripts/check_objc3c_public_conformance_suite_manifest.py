#!/usr/bin/env python3
"""Validate the checked-in stable public conformance suite manifest."""

from __future__ import annotations

import sys
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_shared.schema_registry import validate_registered_schema
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / "tests" / "conformance" / "public_suite_manifest.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "conformance" / "public-suite-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_suite.summary.v1"
SCHEMA_ID = "objc3c-public-conformance-suite-v1"
REQUIRED_PHASES = (
    "parser",
    "sema",
    "lowering",
    "ir",
    "runtime",
    "stdlib",
    "package",
    "release_candidate",
)
REQUIRED_PROFILES = ("core", "stdlib-package", "release-candidate")
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "


def fail(message: str) -> int:
    print(f"objc3c-public-conformance-suite-manifest: FAIL\n- {message}", file=sys.stderr)
    return 1


def repo_path(relative_path: str) -> Path:
    return ROOT / relative_path


def require_file(relative_path: str, *, kind: str) -> Path:
    path = repo_path(relative_path)
    if not path.is_file():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def require_existing_path(relative_path: str, *, kind: str) -> Path:
    path = repo_path(relative_path)
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def require_object(value: Any, *, field: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise RuntimeError(f"{field} must be an object")
    return value


def require_list(value: Any, *, field: str) -> list[Any]:
    if not isinstance(value, list):
        raise RuntimeError(f"{field} must be an array")
    return value


def load_manifest() -> dict[str, Any]:
    manifest = load_json(MANIFEST_PATH)
    validate_registered_schema(manifest, SCHEMA_ID, label=repo_rel(MANIFEST_PATH))
    return manifest


def capability_pairs(matrix_path: str) -> set[tuple[str, str]]:
    matrix = load_json(require_file(matrix_path, kind="capability matrix"))
    pairs: set[tuple[str, str]] = set()
    for row in require_list(matrix.get("capabilities"), field="capability_matrix.capabilities"):
        capability = require_object(row, field="capability row")
        capability_id = capability.get("id")
        if capability.get("state") != "implemented":
            continue
        for support_claim in require_list(
            capability.get("support_claims"),
            field=f"{capability_id}.support_claims",
        ):
            if isinstance(capability_id, str) and isinstance(support_claim, str):
                pairs.add((capability_id, support_claim))
    return pairs


def evidence_map_pairs(evidence_map_path: str) -> set[tuple[str, str]]:
    evidence_map = load_json(require_file(evidence_map_path, kind="evidence map"))
    pairs: set[tuple[str, str]] = set()
    for row in require_list(evidence_map.get("rows"), field="evidence_map.rows"):
        evidence = require_object(row, field="evidence row")
        capability_id = evidence.get("capability_id")
        support_claim = evidence.get("support_claim")
        if isinstance(capability_id, str) and isinstance(support_claim, str):
            pairs.add((capability_id, support_claim))
    return pairs


def support_catalog_claims(catalog_path: str) -> set[str]:
    catalog = load_json(require_file(catalog_path, kind="support claim runnable evidence catalog"))
    claims: set[str] = set()
    for row in require_list(catalog.get("rows"), field="support catalog rows"):
        support_row = require_object(row, field="support catalog row")
        support_claim = support_row.get("support_claim")
        if isinstance(support_claim, str):
            claims.add(support_claim)
    return claims


def validate_source_truth(manifest: dict[str, Any]) -> tuple[set[tuple[str, str]], set[tuple[str, str]], set[str]]:
    source_truth = require_object(manifest.get("source_truth"), field="source_truth")
    if source_truth.get("tmp_source_truth_allowed") is not False:
        raise RuntimeError("source_truth.tmp_source_truth_allowed must be false")
    require_file(str(source_truth["corpus_surface"]), kind="corpus surface")
    require_file(str(source_truth["support_claim_runnable_evidence_catalog"]), kind="support claim catalog")
    matrix_pairs = capability_pairs(str(source_truth["capability_matrix"]))
    evidence_pairs = evidence_map_pairs(str(source_truth["evidence_map"]))
    catalog_claims = support_catalog_claims(str(source_truth["support_claim_runnable_evidence_catalog"]))
    return matrix_pairs, evidence_pairs, catalog_claims


def validate_package_surface(manifest: dict[str, Any]) -> None:
    package_surface = require_object(manifest.get("package_surface"), field="package_surface")
    if package_surface.get("public_command_prefix") != PUBLIC_COMMAND_PREFIX:
        raise RuntimeError("package_surface.public_command_prefix drifted")
    if package_surface.get("offline_compatible") is not True:
        raise RuntimeError("package_surface.offline_compatible must stay true")
    for key in ("artifact_root", "report_root", "package_stage_root"):
        if str(package_surface.get(key, "")).startswith("docs/support/"):
            raise RuntimeError(f"package_surface.{key} cannot be a shared support-doc path")


def normalized_repo_path(relative_path: str) -> str:
    return relative_path.replace("\\", "/").strip("/")


def path_is_inside(relative_path: str, root_path: str) -> bool:
    path = normalized_repo_path(relative_path)
    root = normalized_repo_path(root_path)
    return path == root or path.startswith(f"{root}/")


def validate_fixture_boundary(manifest: dict[str, Any]) -> dict[str, Any]:
    boundary = require_object(manifest.get("fixture_boundary"), field="fixture_boundary")
    public_roots = [
        normalized_repo_path(str(root))
        for root in require_list(boundary.get("public_fixture_roots"), field="public_fixture_roots")
    ]
    internal_roots = [
        normalized_repo_path(str(root))
        for root in require_list(boundary.get("internal_only_roots"), field="internal_only_roots")
    ]
    if len(set(public_roots)) != len(public_roots):
        raise RuntimeError("fixture_boundary.public_fixture_roots contains duplicate roots")
    if len(set(internal_roots)) != len(internal_roots):
        raise RuntimeError("fixture_boundary.internal_only_roots contains duplicate roots")
    for root in public_roots:
        require_existing_path(root, kind="public fixture root")
    for root in internal_roots:
        require_existing_path(root, kind="internal-only fixture root")

    policy = require_object(boundary.get("expected_output_policy"), field="expected_output_policy")
    required_false = (
        "tmp_expected_outputs_allowed",
        "internal_owner_debug_fixtures_public_claim_allowed",
    )
    if policy.get("checked_in_expected_outputs_required") is not True:
        raise RuntimeError("fixture_boundary.expected_output_policy.checked_in_expected_outputs_required must be true")
    for field in required_false:
        if policy.get(field) is not False:
            raise RuntimeError(f"fixture_boundary.expected_output_policy.{field} must be false")
    expected_fields = set(
        str(field)
        for field in require_list(
            policy.get("public_case_expected_fields"),
            field="fixture_boundary.expected_output_policy.public_case_expected_fields",
        )
    )
    missing_expected_fields = {
        "expectation",
        "runnable_command",
        "platform_requirements",
        "positive_evidence",
        "negative_evidence",
    } - expected_fields
    if missing_expected_fields:
        raise RuntimeError(
            "fixture_boundary.expected_output_policy.public_case_expected_fields missing "
            + ", ".join(sorted(missing_expected_fields))
        )
    return {
        "public_fixture_roots": public_roots,
        "internal_only_roots": internal_roots,
        "checked_in_expected_outputs_required": True,
    }


def validate_strict_rejection_policy(manifest: dict[str, Any]) -> dict[str, bool]:
    policy = require_object(manifest.get("strict_rejection_policy"), field="strict_rejection_policy")
    required_false = (
        "compatibility_mode_allowed",
        "fallback_gate_allowed",
        "unsupported_claim_promotion_allowed",
        "tmp_artifact_support_allowed",
    )
    for field in required_false:
        if policy.get(field) is not False:
            raise RuntimeError(f"strict_rejection_policy.{field} must be false")
    fail_closed_states = set(require_list(policy.get("fail_closed_states"), field="fail_closed_states"))
    for state in ("strict-error", "canonical-rejection", "reserved", "unsupported"):
        if state not in fail_closed_states:
            raise RuntimeError(f"strict_rejection_policy.fail_closed_states missing {state}")
    return {field: bool(policy[field]) for field in required_false}


def validate_external_validation_policy(manifest: dict[str, Any]) -> dict[str, Any]:
    policy = require_object(manifest.get("external_validation_policy"), field="external_validation_policy")
    if policy.get("external_evidence_can_create_public_support_claim") is not False:
        raise RuntimeError("external_validation_policy.external_evidence_can_create_public_support_claim must be false")
    if policy.get("tmp_artifact_support_allowed") is not False:
        raise RuntimeError("external_validation_policy.tmp_artifact_support_allowed must be false")

    for command_key in ("replay_command", "publication_command"):
        command = str(policy.get(command_key, ""))
        if not command.startswith(PUBLIC_COMMAND_PREFIX):
            raise RuntimeError(f"external_validation_policy.{command_key} is not public")

    source_surface_path = str(policy["source_surface"])
    trust_policy_path = str(policy["trust_policy"])
    intake_manifest_path = str(policy["intake_manifest"])
    quarantine_manifest_path = str(policy["quarantine_manifest"])
    support_claim_gate_path = str(policy["support_claim_gate"])
    for key, path in (
        ("source_surface", source_surface_path),
        ("trust_policy", trust_policy_path),
        ("intake_manifest", intake_manifest_path),
        ("quarantine_manifest", quarantine_manifest_path),
        ("support_claim_gate", support_claim_gate_path),
    ):
        require_file(path, kind=f"external validation {key}")

    source_surface = load_json(repo_path(source_surface_path))
    trust_policy = load_json(repo_path(trust_policy_path))
    intake_manifest = load_json(repo_path(intake_manifest_path))
    quarantine_manifest = load_json(repo_path(quarantine_manifest_path))
    support_claim_gate = load_json(repo_path(support_claim_gate_path))

    expected_surface_paths = {
        "trust_policy": trust_policy_path,
        "intake_manifest": intake_manifest_path,
        "quarantine_manifest": quarantine_manifest_path,
        "support_claim_gate": support_claim_gate_path,
    }
    for key, expected_path in expected_surface_paths.items():
        if source_surface.get(key) != expected_path:
            raise RuntimeError(f"external_validation_policy.{key} is not mirrored by source_surface")

    admitted_states = set(
        str(state)
        for state in require_list(policy.get("admitted_trust_states"), field="admitted_trust_states")
    )
    rejected_states = set(
        str(state)
        for state in require_list(policy.get("rejected_trust_states"), field="rejected_trust_states")
    )
    if admitted_states != {"accepted"}:
        raise RuntimeError("external_validation_policy.admitted_trust_states must be exactly accepted")
    if not {"candidate", "quarantined", "rejected"}.issubset(rejected_states):
        raise RuntimeError("external_validation_policy.rejected_trust_states must include candidate, quarantined, rejected")
    if set(trust_policy.get("publishable_trust_states", [])) != admitted_states:
        raise RuntimeError("external trust policy publishable states drifted from public suite admission policy")
    if set(trust_policy.get("capability_truth_trust_states", [])) != admitted_states:
        raise RuntimeError("external trust policy capability truth states drifted from public suite admission policy")
    if not rejected_states.issubset(set(trust_policy.get("forbidden_capability_truth_states", []))):
        raise RuntimeError("external trust policy does not forbid all public suite rejected states")

    if support_claim_gate.get("trust_policy") != trust_policy_path:
        raise RuntimeError("external support claim gate trust_policy path drifted")
    if support_claim_gate.get("intake_manifest") != intake_manifest_path:
        raise RuntimeError("external support claim gate intake_manifest path drifted")

    accepted_count = 0
    for raw_entry in require_list(intake_manifest.get("entries"), field="external intake entries"):
        entry = require_object(raw_entry, field="external intake entry")
        fixture_id = str(entry.get("fixture_id", ""))
        if entry.get("trust_state") not in admitted_states:
            raise RuntimeError(f"{fixture_id} is in intake manifest but is not admitted")
        accepted_count += 1
        replay_script = str(entry.get("replay_script", ""))
        require_file(replay_script, kind=f"{fixture_id} replay script")
        normalized_path = str(entry.get("normalized_case_path") or entry.get("normalized_contract_path") or "")
        require_file(normalized_path, kind=f"{fixture_id} normalized replay anchor")
        owner_contract = require_object(entry.get("owner_contract"), field=f"{fixture_id}.owner_contract")
        for field in (
            "local_only_capability_truth_allowed",
            "evidence_log_capability_truth_allowed",
            "retired_route_trust_route_allowed",
        ):
            if owner_contract.get(field) is not False:
                raise RuntimeError(f"{fixture_id}.owner_contract.{field} must be false")

    rejected_count = 0
    for raw_entry in require_list(quarantine_manifest.get("entries"), field="external quarantine entries"):
        entry = require_object(raw_entry, field="external quarantine entry")
        fixture_id = str(entry.get("fixture_id", ""))
        if entry.get("trust_state") not in rejected_states:
            raise RuntimeError(f"{fixture_id} has a quarantine trust_state not rejected by public suite policy")
        rejected_count += 1
        require_file(str(entry.get("replay_script", "")), kind=f"{fixture_id} quarantine replay script")
        require_file(str(entry.get("normalized_contract_path", "")), kind=f"{fixture_id} quarantine replay anchor")
        owner_contract = require_object(entry.get("owner_contract"), field=f"{fixture_id}.owner_contract")
        if owner_contract.get("capability_truth_allowed") is not False:
            raise RuntimeError(f"{fixture_id}.owner_contract.capability_truth_allowed must be false")
        if owner_contract.get("retired_route_trust_route_allowed") is not False:
            raise RuntimeError(f"{fixture_id}.owner_contract.retired_route_trust_route_allowed must be false")

    return {
        "accepted_external_validation_entries": accepted_count,
        "rejected_external_validation_entries": rejected_count,
        "external_validation_replay_command": policy["replay_command"],
        "external_validation_publication_command": policy["publication_command"],
    }


def validate_phase_and_profiles(manifest: dict[str, Any]) -> tuple[set[str], set[str]]:
    phase_rows = require_list(manifest.get("phase_taxonomy"), field="phase_taxonomy")
    phase_ids = [require_object(row, field="phase row").get("phase_id") for row in phase_rows]
    if len(set(phase_ids)) != len(phase_ids):
        raise RuntimeError("phase_taxonomy contains duplicate phase_id values")
    missing_phases = [phase for phase in REQUIRED_PHASES if phase not in phase_ids]
    if missing_phases:
        raise RuntimeError(f"phase_taxonomy missing required phases: {', '.join(missing_phases)}")
    for row in phase_rows:
        phase = require_object(row, field="phase row")
        require_file(str(phase["source_manifest"]), kind=f"{phase['phase_id']} phase source manifest")
        if phase.get("required_for_release_candidate") is not True:
            raise RuntimeError(f"{phase['phase_id']} must remain release-candidate required")

    profile_rows = require_list(manifest.get("public_profiles"), field="public_profiles")
    profile_ids = [require_object(row, field="profile row").get("profile_id") for row in profile_rows]
    if len(set(profile_ids)) != len(profile_ids):
        raise RuntimeError("public_profiles contains duplicate profile_id values")
    missing_profiles = [profile for profile in REQUIRED_PROFILES if profile not in profile_ids]
    if missing_profiles:
        raise RuntimeError(f"public_profiles missing required profiles: {', '.join(missing_profiles)}")
    phase_id_set = {str(phase_id) for phase_id in phase_ids}
    for row in profile_rows:
        profile = require_object(row, field="profile row")
        command = str(profile["default_command"])
        if not command.startswith(PUBLIC_COMMAND_PREFIX):
            raise RuntimeError(f"{profile['profile_id']} default command is not public")
        for phase_id in require_list(profile["required_phase_ids"], field=f"{profile['profile_id']}.required_phase_ids"):
            if phase_id not in phase_id_set:
                raise RuntimeError(f"{profile['profile_id']} references unknown phase {phase_id}")
    return phase_id_set, {str(profile_id) for profile_id in profile_ids}


def validate_public_suite_source_path(
    relative_path: str,
    *,
    case_id: str,
    public_roots: list[str],
    internal_roots: list[str],
) -> None:
    path = normalized_repo_path(relative_path)
    if path.startswith("tmp/"):
        raise RuntimeError(f"{case_id} uses tmp as source truth: {path}")
    for root in internal_roots:
        if path_is_inside(path, root):
            raise RuntimeError(f"{case_id} uses internal-only public-suite source path: {path}")
    if not any(path_is_inside(path, root) for root in public_roots):
        raise RuntimeError(f"{case_id} source path is outside public fixture roots: {path}")


def validate_case_paths(
    case: dict[str, Any],
    *,
    public_roots: list[str],
    internal_roots: list[str],
) -> None:
    for key in ("source_manifest", "conformance_fixture", "traceability_fixture"):
        path = str(case[key])
        validate_public_suite_source_path(
            path,
            case_id=str(case["case_id"]),
            public_roots=public_roots,
            internal_roots=internal_roots,
        )
        require_file(path, kind=f"{case['case_id']} {key}")
    for key in ("positive_evidence", "negative_evidence"):
        for evidence_path in require_list(case[key], field=f"{case['case_id']}.{key}"):
            path = str(evidence_path)
            validate_public_suite_source_path(
                path,
                case_id=str(case["case_id"]),
                public_roots=public_roots,
                internal_roots=internal_roots,
            )
            require_file(path, kind=f"{case['case_id']} {key}")


def validate_cases(
    manifest: dict[str, Any],
    *,
    phase_ids: set[str],
    profile_ids: set[str],
    matrix_pairs: set[tuple[str, str]],
    evidence_pairs: set[tuple[str, str]],
    catalog_claims: set[str],
    fixture_boundary: dict[str, Any],
) -> dict[str, Any]:
    cases = require_list(manifest.get("suite_cases"), field="suite_cases")
    seen_case_ids: set[str] = set()
    phase_counts: Counter[str] = Counter()
    profile_counts: Counter[str] = Counter()
    public_commands: set[str] = set()
    capability_ids: set[str] = set()
    support_claims: set[str] = set()
    strict_case_count = 0
    release_gate_count = 0
    catalog_backed_count = 0

    for raw_case in cases:
        case = require_object(raw_case, field="suite case")
        case_id = str(case["case_id"])
        if case_id in seen_case_ids:
            raise RuntimeError(f"duplicate suite case: {case_id}")
        seen_case_ids.add(case_id)

        phase = str(case["phase"])
        if phase not in phase_ids:
            raise RuntimeError(f"{case_id} references unknown phase {phase}")
        phase_counts[phase] += 1

        for profile_id in require_list(case["profile_ids"], field=f"{case_id}.profile_ids"):
            if profile_id not in profile_ids:
                raise RuntimeError(f"{case_id} references unknown profile {profile_id}")
            profile_counts[str(profile_id)] += 1

        command = str(case["runnable_command"])
        if not command.startswith(PUBLIC_COMMAND_PREFIX):
            raise RuntimeError(f"{case_id} runnable command is not public: {command}")
        public_commands.add(command)

        if case.get("packaging_class") != "public-stable":
            raise RuntimeError(f"{case_id} is not in the public-stable packaging class")
        if case.get("release_gate") is True:
            release_gate_count += 1

        capability_id = str(case["capability_id"])
        support_claim = str(case["support_claim"])
        pair = (capability_id, support_claim)
        if pair not in matrix_pairs:
            raise RuntimeError(f"{case_id} lacks implemented capability matrix support for {support_claim}")
        if pair not in evidence_pairs:
            raise RuntimeError(f"{case_id} lacks evidence-map support for {support_claim}")
        if support_claim in catalog_claims:
            catalog_backed_count += 1
        capability_ids.add(capability_id)
        support_claims.add(support_claim)

        validate_case_paths(
            case,
            public_roots=fixture_boundary["public_fixture_roots"],
            internal_roots=fixture_boundary["internal_only_roots"],
        )
        if case["expectation"] in {"strict-error", "canonical-rejection"}:
            strict_case_count += 1

    missing_case_phases = [phase for phase in REQUIRED_PHASES if phase_counts[phase] == 0]
    if missing_case_phases:
        raise RuntimeError(f"suite_cases missing required phase coverage: {', '.join(missing_case_phases)}")
    if strict_case_count == 0:
        raise RuntimeError("suite_cases must include at least one strict-error or canonical-rejection case")
    if profile_counts["release-candidate"] == 0:
        raise RuntimeError("suite_cases missing release-candidate profile coverage")
    if release_gate_count != len(cases):
        raise RuntimeError("every public suite case must remain a release gate")

    return {
        "case_count": len(cases),
        "phase_case_counts": dict(sorted(phase_counts.items())),
        "profile_case_counts": dict(sorted(profile_counts.items())),
        "strict_or_rejection_case_count": strict_case_count,
        "release_gate_case_count": release_gate_count,
        "capability_matrix_backed_case_count": len(cases),
        "evidence_map_backed_case_count": len(cases),
        "support_claim_catalog_backed_case_count": catalog_backed_count,
        "capability_ids": sorted(capability_ids),
        "support_claims": sorted(support_claims),
        "public_commands": sorted(public_commands),
    }


def validate_release_candidate_profile(manifest: dict[str, Any]) -> dict[str, Any]:
    release_profile = require_object(
        manifest.get("release_candidate_profile"),
        field="release_candidate_profile",
    )
    if release_profile.get("profile_id") != "release-candidate":
        raise RuntimeError("release_candidate_profile.profile_id must be release-candidate")
    if release_profile.get("requires_all_public_stable_cases") is not True:
        raise RuntimeError("release_candidate_profile.requires_all_public_stable_cases must be true")
    if release_profile.get("consumes_external_validation_policy") is not True:
        raise RuntimeError("release_candidate_profile.consumes_external_validation_policy must be true")
    if release_profile.get("tmp_artifact_support_allowed") is not False:
        raise RuntimeError("release_candidate_profile.tmp_artifact_support_allowed must be false")
    for command_key in ("gate_command", "runnable_gate_command", "scorecard_command"):
        command = str(release_profile.get(command_key, ""))
        if not command.startswith(PUBLIC_COMMAND_PREFIX):
            raise RuntimeError(f"release_candidate_profile.{command_key} is not public")

    required_phase_ids = [
        str(phase_id)
        for phase_id in require_list(
            release_profile.get("required_phase_ids"),
            field="release_candidate_profile.required_phase_ids",
        )
    ]
    if set(required_phase_ids) != set(REQUIRED_PHASES):
        raise RuntimeError("release_candidate_profile.required_phase_ids drifted from required public phases")

    profiles = {
        str(require_object(profile, field="public profile")["profile_id"]): require_object(
            profile, field="public profile"
        )
        for profile in require_list(manifest.get("public_profiles"), field="public_profiles")
    }
    public_release_profile = profiles.get("release-candidate")
    if public_release_profile is None:
        raise RuntimeError("public_profiles missing release-candidate profile")
    if public_release_profile.get("default_command") != release_profile.get("gate_command"):
        raise RuntimeError("release-candidate default command drifted from release_candidate_profile.gate_command")

    package_case_id = str(release_profile.get("package_replay_case_id", ""))
    public_stable_count = 0
    release_case_count = 0
    package_case_found = False
    for raw_case in require_list(manifest.get("suite_cases"), field="suite_cases"):
        case = require_object(raw_case, field="suite case")
        if case.get("packaging_class") == "public-stable":
            public_stable_count += 1
            if "release-candidate" not in require_list(case.get("profile_ids"), field=f"{case['case_id']}.profile_ids"):
                raise RuntimeError(f"{case['case_id']} is public-stable but missing release-candidate profile")
        if "release-candidate" in case.get("profile_ids", []):
            release_case_count += 1
        if case.get("case_id") == package_case_id:
            package_case_found = True
            if "release-candidate" not in case.get("profile_ids", []):
                raise RuntimeError("release_candidate_profile.package_replay_case_id is not a release-candidate case")
    if not package_case_found:
        raise RuntimeError("release_candidate_profile.package_replay_case_id does not exist")
    if public_stable_count != release_case_count:
        raise RuntimeError("release candidate profile must consume every public-stable case")

    return {
        "release_candidate_gate_command": release_profile["gate_command"],
        "release_candidate_runnable_gate_command": release_profile["runnable_gate_command"],
        "release_candidate_scorecard_command": release_profile["scorecard_command"],
        "release_candidate_public_stable_case_count": public_stable_count,
        "release_candidate_required_phase_count": len(required_phase_ids),
    }


def main() -> int:
    try:
        manifest = load_manifest()
        matrix_pairs, evidence_pairs, catalog_claims = validate_source_truth(manifest)
        validate_package_surface(manifest)
        fixture_boundary = validate_fixture_boundary(manifest)
        external_validation_summary = validate_external_validation_policy(manifest)
        strict_rejection_flags = validate_strict_rejection_policy(manifest)
        phase_ids, profile_ids = validate_phase_and_profiles(manifest)
        case_summary = validate_cases(
            manifest,
            phase_ids=phase_ids,
            profile_ids=profile_ids,
            matrix_pairs=matrix_pairs,
            evidence_pairs=evidence_pairs,
            catalog_claims=catalog_claims,
            fixture_boundary=fixture_boundary,
        )
        release_candidate_summary = validate_release_candidate_profile(manifest)
    except Exception as exc:
        return fail(str(exc))

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "manifest_path": repo_rel(MANIFEST_PATH),
        "schema_id": SCHEMA_ID,
        "suite_id": manifest["suite_id"],
        "suite_version": manifest["suite_version"],
        "public_status": manifest["public_status"],
        "packageable": True,
        "phase_count": len(phase_ids),
        "profile_count": len(profile_ids),
        "required_phases": list(REQUIRED_PHASES),
        "required_profiles": list(REQUIRED_PROFILES),
        "strict_rejection_flags": strict_rejection_flags,
        "fixture_boundary": {
            "public_fixture_root_count": len(fixture_boundary["public_fixture_roots"]),
            "internal_only_root_count": len(fixture_boundary["internal_only_roots"]),
            "checked_in_expected_outputs_required": fixture_boundary[
                "checked_in_expected_outputs_required"
            ],
        },
        **external_validation_summary,
        **release_candidate_summary,
        **case_summary,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-public-conformance-suite-manifest: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
