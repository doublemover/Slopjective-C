#!/usr/bin/env python3
"""Validate issue-owned platform support source truth for OBJ3-NEXT-024."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
import sys
from typing import Any, Iterable

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.objc3c_shared.json_io import (
    load_json_object,
    validate_json_schema,
    write_json_file,
)
from scripts.objc3c_tooling.paths import repo_rel, resolve_repo_path
from scripts.objc3c_workflow import ACTION_SPECS
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS

SOURCE_TRUTH_PATH = ROOT / "tests" / "tooling" / "fixtures" / "platform_support" / "source_truth_matrix.json"
SCHEMA_PATH = ROOT / "schemas" / "objc3c-platform-support-source-truth-v1.schema.json"
DEFAULT_SUMMARY_PATH = ROOT / "tmp" / "reports" / "platform-matrix" / "matrix-validation-summary.json"

REQUIRED_SUPPORTED_EVIDENCE_CLASSES = ("build", "package", "install", "execution")
REQUIRED_AUXILIARY_EVIDENCE_CLASSES = ("toolchain", "hosted_ci", "clean_room")
REQUIRED_TOOLCHAIN_COMPONENTS = ("llvm", "clang", "cmake", "ninja", "python", "node", "pwsh")
FORBIDDEN_SOURCE_PREFIXES = ("tmp/", "artifacts/")
FORBIDDEN_RANGE_TERMS = ("all", "best effort", "best-effort", "compat", "fallback")


@dataclass(frozen=True)
class ValidationInputs:
    source_truth: dict[str, Any]
    supported_platforms: dict[str, Any]
    platform_evidence: dict[str, Any]
    unsupported_host_policy: dict[str, Any]


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def _load_source_truth(path: Path = SOURCE_TRUTH_PATH) -> dict[str, Any]:
    payload = load_json_object(path)
    schema_path = resolve_repo_path(str(payload.get("schema_path", SCHEMA_PATH)))
    schema = load_json_object(schema_path)
    label = repo_rel(path) if path.resolve().is_relative_to(ROOT.resolve()) else path.as_posix()
    validate_json_schema(payload, schema, label=label)
    if path.resolve() == SOURCE_TRUTH_PATH.resolve():
        expect(Path(str(payload["source_path"])).as_posix() == repo_rel(path), "source_path does not point at the loaded source-truth fixture")
    return payload


def _load_inputs(source_truth_path: Path = SOURCE_TRUTH_PATH) -> ValidationInputs:
    source_truth = _load_source_truth(source_truth_path)
    upstream = source_truth["upstream_sources"]
    return ValidationInputs(
        source_truth=source_truth,
        supported_platforms=load_json_object(resolve_repo_path(upstream["supported_platforms"])),
        platform_evidence=load_json_object(resolve_repo_path(upstream["platform_toolchain_support_evidence"])),
        unsupported_host_policy=load_json_object(resolve_repo_path(upstream["unsupported_host_policy"])),
    )


def _evidence_by_id(platform_evidence: dict[str, Any]) -> dict[str, dict[str, Any]]:
    by_id: dict[str, dict[str, Any]] = {}
    for record in platform_evidence.get("evidence_records", []):
        evidence_id = str(record.get("evidence_id", ""))
        expect(evidence_id, "platform evidence record missing evidence_id")
        expect(evidence_id not in by_id, f"duplicate platform evidence id: {evidence_id}")
        by_id[evidence_id] = record
    return by_id


def _action_from_public_command(command: str) -> str:
    prefix = "npm run objc3c -- "
    expect(command.startswith(prefix), f"replay command is not on public objc3c bridge: {command}")
    action = command[len(prefix) :].split()[0]
    expect(action, f"replay command missing action: {command}")
    return action


def _require_public_command_registered(command: str) -> str:
    action = _action_from_public_command(command)
    expect(action in ACTION_SPECS, f"public replay action is not in ACTION_SPECS: {action}")
    expect(action in ACTION_HANDLERS, f"public replay action is not in ACTION_HANDLERS: {action}")
    return action


def _require_source_paths_checked_in(record: dict[str, Any]) -> None:
    evidence_id = str(record["evidence_id"])
    source_paths = [str(path).replace("\\", "/") for path in record.get("source_paths", [])]
    expect(source_paths, f"{evidence_id} missing checked source paths")
    for source_path in source_paths:
        expect(
            not source_path.startswith(FORBIDDEN_SOURCE_PREFIXES),
            f"{evidence_id} used generated output as source truth: {source_path}",
        )
        expect(resolve_repo_path(source_path).is_file(), f"{evidence_id} source path does not exist: {source_path}")


def _require_generated_paths_are_not_source(record: dict[str, Any]) -> None:
    evidence_id = str(record["evidence_id"])
    generated_paths = [str(path).replace("\\", "/") for path in record.get("generated_report_paths", [])]
    expect(generated_paths, f"{evidence_id} missing generated report path for replay output")
    for generated_path in generated_paths:
        expect(
            generated_path.startswith(("tmp/", "artifacts/")),
            f"{evidence_id} generated report path is not under tmp/ or artifacts/: {generated_path}",
        )


def _require_supporting_record(
    records_by_id: dict[str, dict[str, Any]],
    evidence_id: str,
    *,
    platform_id: str,
    evidence_class: str,
) -> dict[str, Any]:
    expect(evidence_id in records_by_id, f"{platform_id} missing evidence record {evidence_id}")
    record = records_by_id[evidence_id]
    expect(record.get("claim_weight") == "supporting", f"{evidence_id} is not supporting evidence")
    expect(record.get("evidence_class") == evidence_class, f"{evidence_id} expected {evidence_class} evidence")
    expect(record.get("requires_network") is False, f"{evidence_id} requires network for a support claim")
    expect(record.get("unsupported_host_behavior") == "fail-closed", f"{evidence_id} does not fail closed for unsupported hosts")
    expect(platform_id in record.get("supports_platform_ids", []), f"{evidence_id} does not support {platform_id}")
    _require_source_paths_checked_in(record)
    _require_generated_paths_are_not_source(record)
    replay_commands = [str(command) for command in record.get("replay_commands", [])]
    expect(replay_commands, f"{evidence_id} missing replay commands")
    for command in replay_commands:
        if command.startswith("npm run objc3c -- "):
            _require_public_command_registered(command)
    return record


def _require_policy_record(records_by_id: dict[str, dict[str, Any]], evidence_id: str) -> dict[str, Any]:
    expect(evidence_id in records_by_id, f"missing policy evidence record {evidence_id}")
    record = records_by_id[evidence_id]
    expect(record.get("claim_weight") == "policy", f"{evidence_id} must remain policy evidence")
    expect(record.get("requires_network") is False, f"{evidence_id} policy evidence requires network")
    expect(record.get("unsupported_host_behavior") == "fail-closed", f"{evidence_id} policy evidence is not fail-closed")
    expect(not record.get("supports_platform_ids"), f"{evidence_id} policy evidence widened support")
    _require_source_paths_checked_in(record)
    _require_generated_paths_are_not_source(record)
    return record


def _supported_platforms_by_id(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(platform["platform_id"]): platform
        for platform in payload.get("supported_platforms", [])
        if isinstance(platform, dict) and platform.get("platform_id")
    }


def _upstream_support_rows_by_id(platform_evidence: dict[str, Any], support_state: str) -> dict[str, dict[str, Any]]:
    return {
        str(row["platform_id"]): row
        for row in platform_evidence.get("support_rows", [])
        if isinstance(row, dict) and row.get("support_state") == support_state
    }


def _unsupported_failure_ids(policy: dict[str, Any]) -> set[str]:
    return {
        str(check["failure_id"])
        for check in policy.get("synthetic_unsupported_host_checks", [])
        if isinstance(check, dict) and check.get("failure_id")
    }


def _require_public_commands(commands: Iterable[str]) -> set[str]:
    actions: set[str] = set()
    for command in commands:
        actions.add(_require_public_command_registered(str(command)))
    return actions


def _validate_supported_rows(inputs: ValidationInputs, records_by_id: dict[str, dict[str, Any]]) -> list[str]:
    source_truth = inputs.source_truth
    supported_by_id = _supported_platforms_by_id(inputs.supported_platforms)
    upstream_rows = _upstream_support_rows_by_id(inputs.platform_evidence, "supported")
    supported_ids: list[str] = []

    expect(
        tuple(source_truth["required_supported_evidence_classes"]) == REQUIRED_SUPPORTED_EVIDENCE_CLASSES,
        "supported evidence class requirements drifted",
    )
    expect(
        tuple(source_truth["required_auxiliary_evidence_classes"]) == REQUIRED_AUXILIARY_EVIDENCE_CLASSES,
        "auxiliary evidence class requirements drifted",
    )
    expect(
        tuple(source_truth["required_toolchain_components"]) == REQUIRED_TOOLCHAIN_COMPONENTS,
        "toolchain component requirements drifted",
    )

    for row in source_truth["supported_rows"]:
        platform_id = str(row["platform_id"])
        supported_ids.append(platform_id)
        expect(platform_id in supported_by_id, f"{platform_id} is not in supported_platforms.json")
        expect(platform_id in upstream_rows, f"{platform_id} is not in upstream platform support evidence")
        upstream_row = upstream_rows[platform_id]
        expect(row["row_id"] == upstream_row["row_id"], f"{platform_id} row_id drifted from upstream evidence")
        expect(row["host_os"] == supported_by_id[platform_id]["host_os"], f"{platform_id} host_os drifted")
        expect(row["host_arch"] == supported_by_id[platform_id]["host_arch"], f"{platform_id} host_arch drifted")
        expect(row["required_evidence"] == upstream_row["evidence"], f"{platform_id} evidence ids drifted from upstream evidence")
        expect(row["toolchain_evidence_ids"] == upstream_row["toolchain_evidence_ids"], f"{platform_id} toolchain evidence ids drifted")
        expect(row["hosted_ci_evidence_ids"] == upstream_row["hosted_ci_evidence_ids"], f"{platform_id} hosted CI evidence ids drifted")
        expect(row["local_clean_room_evidence_ids"] == upstream_row["local_clean_room_evidence_ids"], f"{platform_id} clean-room evidence ids drifted")

        commands_seen = _require_public_commands(row["public_replay_commands"])
        for evidence_class, evidence_id in row["required_evidence"].items():
            record = _require_supporting_record(
                records_by_id,
                str(evidence_id),
                platform_id=platform_id,
                evidence_class=str(evidence_class),
            )
            commands_seen.update(_require_public_commands(record["replay_commands"]))

        for evidence_id in row["toolchain_evidence_ids"]:
            record = _require_supporting_record(records_by_id, str(evidence_id), platform_id=platform_id, evidence_class="toolchain")
            commands_seen.update(
                _require_public_commands(command for command in record["replay_commands"] if str(command).startswith("npm run objc3c -- "))
            )
        for evidence_id in row["hosted_ci_evidence_ids"]:
            record = _require_supporting_record(records_by_id, str(evidence_id), platform_id=platform_id, evidence_class="hosted_ci")
            commands_seen.update(_require_public_commands(record["replay_commands"]))
        for evidence_id in row["local_clean_room_evidence_ids"]:
            record = _require_supporting_record(records_by_id, str(evidence_id), platform_id=platform_id, evidence_class="clean_room")
            commands_seen.update(_require_public_commands(record["replay_commands"]))

        for required_action in (
            "build-native-binaries",
            "package-runnable-toolchain",
            "validate-packaging-channels-end-to-end",
            "test-execution-smoke",
            "test-hosted-execution-smoke",
            "validate-package-install-distribution",
            "build-platform-support-matrix",
        ):
            expect(required_action in commands_seen, f"{platform_id} missing public replay action {required_action}")

    expect(sorted(supported_ids) == sorted(supported_by_id), "source-truth supported rows drifted from supported_platforms.json")
    return supported_ids


def _validate_unsupported_rows(inputs: ValidationInputs, records_by_id: dict[str, dict[str, Any]], supported_ids: set[str]) -> list[str]:
    upstream_rows = _upstream_support_rows_by_id(inputs.platform_evidence, "unsupported")
    failure_ids = _unsupported_failure_ids(inputs.unsupported_host_policy)
    unsupported_ids: list[str] = []
    for row in inputs.source_truth["unsupported_rows"]:
        platform_id = str(row["platform_id"])
        unsupported_ids.append(platform_id)
        expect(platform_id not in supported_ids, f"{platform_id} is both supported and unsupported")
        expect(platform_id in upstream_rows, f"{platform_id} is not in upstream unsupported evidence")
        upstream_row = upstream_rows[platform_id]
        expect(row["row_id"] == upstream_row["row_id"], f"{platform_id} row_id drifted from upstream unsupported evidence")
        expect(row["failure_id"] == upstream_row["failure_id"], f"{platform_id} failure_id drifted from upstream unsupported evidence")
        expect(row["failure_id"] in failure_ids, f"{platform_id} failure_id not in unsupported host policy")
        expect(row["fail_closed_evidence_id"] == upstream_row["evidence"]["fail_closed"], f"{platform_id} fail-closed evidence drifted")
        _require_policy_record(records_by_id, row["fail_closed_evidence_id"])
        _require_public_commands(row["public_replay_commands"])
    return unsupported_ids


def _validate_toolchain_ranges(inputs: ValidationInputs, records_by_id: dict[str, dict[str, Any]], supported_ids: set[str]) -> list[str]:
    components_seen: list[str] = []
    for toolchain_range in inputs.platform_evidence.get("toolchain_ranges", []):
        component = str(toolchain_range.get("component", ""))
        components_seen.append(component)
        expect(toolchain_range.get("unsupported_version_behavior") == "fail-closed-no-range-claim", f"{component} toolchain range does not fail closed")
        range_claim = str(toolchain_range.get("range_claim", "")).lower()
        for forbidden_term in FORBIDDEN_RANGE_TERMS:
            expect(forbidden_term not in range_claim, f"{component} toolchain range used unsupported compatibility language: {forbidden_term}")
        platform_ids = {str(platform_id) for platform_id in toolchain_range.get("platform_ids", [])}
        if toolchain_range.get("claim_state") == "evidence-bound":
            expect(platform_ids, f"{component} evidence-bound range missing platform ids")
            expect(platform_ids <= supported_ids, f"{component} range widened support to unsupported platforms: {sorted(platform_ids - supported_ids)}")
            for evidence_id in toolchain_range.get("evidence_ids", []):
                for platform_id in platform_ids:
                    _require_supporting_record(records_by_id, str(evidence_id), platform_id=platform_id, evidence_class="toolchain")
            continue
        expect(toolchain_range.get("claim_state") == "reserved", f"{component} toolchain range used unknown claim state")
        expect(not platform_ids, f"{component} reserved range cannot list platform ids")
        for evidence_id in toolchain_range.get("evidence_ids", []):
            _require_policy_record(records_by_id, str(evidence_id))

    expect(sorted(components_seen) == sorted(REQUIRED_TOOLCHAIN_COMPONENTS), "toolchain range components drifted")
    return components_seen


def validate_platform_support_source_truth(source_truth_path: Path = SOURCE_TRUTH_PATH) -> dict[str, Any]:
    inputs = _load_inputs(source_truth_path)
    policy = inputs.source_truth["claim_policy"]
    expect(policy["supported_rows_require_checked_source"] is True, "supported rows must require checked source")
    expect(policy["support_claims_require_live_network"] is False, "support claims cannot require live network")
    expect(policy["generated_reports_are_source_truth"] is False, "generated reports cannot be source truth")
    expect(policy["unsupported_host_result"] == "fail-closed", "unsupported host behavior must fail closed")
    expect(policy["unsupported_toolchain_result"] == "fail-closed-no-range-claim", "unsupported toolchain behavior must fail closed")

    records_by_id = _evidence_by_id(inputs.platform_evidence)
    supported_ids = set(_validate_supported_rows(inputs, records_by_id))
    unsupported_ids = _validate_unsupported_rows(inputs, records_by_id, supported_ids)
    toolchain_components = _validate_toolchain_ranges(inputs, records_by_id, supported_ids)

    return {
        "contract_id": "objc3c.platform.support.source_truth.validation.summary.v1",
        "status": "PASS",
        "source_path": repo_rel(source_truth_path) if source_truth_path.resolve().is_relative_to(ROOT.resolve()) else source_truth_path.as_posix(),
        "schema_path": inputs.source_truth["schema_path"],
        "issue": inputs.source_truth["issue"],
        "supported_platform_ids": sorted(supported_ids),
        "unsupported_platform_ids": sorted(unsupported_ids),
        "required_supported_evidence_classes": list(REQUIRED_SUPPORTED_EVIDENCE_CLASSES),
        "required_auxiliary_evidence_classes": list(REQUIRED_AUXILIARY_EVIDENCE_CLASSES),
        "required_toolchain_components": list(REQUIRED_TOOLCHAIN_COMPONENTS),
        "validated_toolchain_components": sorted(toolchain_components),
        "public_action_count": len(ACTION_SPECS),
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-truth", type=Path, default=SOURCE_TRUTH_PATH)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_PATH)
    args = parser.parse_args(argv)

    summary = validate_platform_support_source_truth(args.source_truth)
    write_json_file(args.summary_out, summary, sort_keys=True)
    print(f"summary_path: {repo_rel(args.summary_out)}")
    print("objc3c-platform-support-source-truth: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
