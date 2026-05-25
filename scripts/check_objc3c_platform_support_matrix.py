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
from scripts.platform_hardening_contracts.host_evidence_contract import (
    HOST_EVIDENCE_REPORT_ROOT,
    HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE,
    host_evidence_review_candidate_path_for_platform,
)

SOURCE_TRUTH_PATH = ROOT / "tests" / "tooling" / "fixtures" / "platform_support" / "source_truth_matrix.json"
SCHEMA_PATH = ROOT / "schemas" / "objc3c-platform-support-source-truth-v1.schema.json"
DEFAULT_SUMMARY_PATH = (
    ROOT / "tmp" / "reports" / "platform-hardening" / "platform-matrix-summary.json"
)

REQUIRED_SUPPORTED_EVIDENCE_CLASSES = ("build", "package", "install", "execution")
REQUIRED_AUXILIARY_EVIDENCE_CLASSES = ("toolchain", "hosted_ci", "clean_room")
REQUIRED_TOOLCHAIN_COMPONENTS = ("llvm", "clang", "cmake", "ninja", "python", "node", "pwsh")
REQUIRED_ROADMAP_ISSUE_REFS = (8206, 8228, 8229, 8230, 8231, 8232)
EXPECTED_UNSUPPORTED_PLATFORM_ISSUES = {
    "linux-x64": 8228,
    "darwin-arm64": 8229,
}
EXPECTED_SANITIZER_ISSUES = {
    "address": 8230,
    "undefined": 8231,
}
PLATFORM_HOST_ISSUE_REFS = (8228, 8229)
EXPECTED_FIXED_UMBRELLA_CHILD_ISSUE_CONTRACTS = {
    8230: {
        "row_id": "objc3c.package.sanitizer.asan.reserved",
        "contract_kind": "sanitizer-runtime-package",
        "claim_state": "evidence-bound",
        "required_promotion_evidence": ("package", "install", "execution"),
    },
    8231: {
        "row_id": "objc3c.package.sanitizer.ubsan.reserved",
        "contract_kind": "sanitizer-runtime-package",
        "claim_state": "evidence-bound",
        "required_promotion_evidence": ("package", "install", "execution"),
    },
    8232: {
        "row_id": "objc3c.llvm.native-object-emission.fail-closed.v1",
        "contract_kind": "native-object-emission",
        "claim_state": "toolchain-prerequisite-fail-closed",
        "required_promotion_evidence": ("toolchain", "native-object-emission"),
    },
}
REQUIRED_HOST_PROMOTION_SOURCE_SECTIONS = (
    "host_identity_records",
    "toolchain_probe_records",
    "package_root_evidence_records",
    "native_execution_evidence_records",
    "object_identity_records",
    "debug_identity_records",
    "package_install_identity_records",
    "runtime_load_link_proof_records",
    "negative_host_toolchain_cases",
)
HOST_EVIDENCE_WORKFLOW_PATH = ".github/workflows/platform-host-evidence.yml"
HOST_EVIDENCE_DISPATCH_GATEWAY_WORKFLOW_PATHS = (
    ".github/workflows/conformance-minima.yml",
)
HOST_EVIDENCE_ACCEPTED_WORKFLOW_PATHS = (
    HOST_EVIDENCE_WORKFLOW_PATH,
    *HOST_EVIDENCE_DISPATCH_GATEWAY_WORKFLOW_PATHS,
)
HOST_EVIDENCE_INGESTION_ACTION = "ingest-platform-host-evidence"
HOST_EVIDENCE_REVIEW_ACTION = "review-platform-host-evidence"
HOST_EVIDENCE_INGESTION_HELPER = "scripts/ingest_objc3c_platform_host_evidence.py"
HOST_EVIDENCE_REVIEW_HELPER = "scripts/review_objc3c_platform_host_evidence.py"
HOST_EVIDENCE_CANDIDATE_RECORD_IDS = (
    "objc3c.evidence.hosted-ci.linux-x64.generated-host-run",
    "objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run",
)
HOST_EVIDENCE_RUNNER_LABELS = {
    "linux-x64": "ubuntu-24.04",
    "darwin-arm64": "macos-15",
}
REQUIRED_NEGATIVE_HOST_TOOLCHAIN_CASES = (
    "objc3c.negative.host.linux-x64.no-native-execution",
    "objc3c.negative.host.linux-x64.generated-host-evidence-no-promotion",
    "objc3c.negative.host.darwin-arm64.no-native-execution",
    "objc3c.negative.host.darwin-arm64.generated-host-evidence-no-promotion",
    "objc3c.negative.toolchain.missing-llc",
    "objc3c.negative.toolchain.mixed-root",
    "objc3c.negative.toolchain.mismatched-version",
    "objc3c.negative.toolchain.unsupported-version",
)
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


def _upstream_package_rows_by_id(platform_evidence: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row["row_id"]): row
        for row in platform_evidence.get("package_variant_rows", [])
        if isinstance(row, dict) and row.get("row_id")
    }


def _upstream_sanitizer_rows_by_id(platform_evidence: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row["variant_id"]): row
        for row in platform_evidence.get("sanitizer_variants", [])
        if isinstance(row, dict) and row.get("variant_id")
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
        expect(row["issue_ref"] == upstream_row["issue_ref"], f"{platform_id} issue_ref drifted from upstream evidence")
        expect(row["host_os"] == supported_by_id[platform_id]["host_os"], f"{platform_id} host_os drifted")
        expect(row["host_arch"] == supported_by_id[platform_id]["host_arch"], f"{platform_id} host_arch drifted")
        expect(row["required_evidence"] == upstream_row["evidence"], f"{platform_id} evidence ids drifted from upstream evidence")
        expect(row["package_variant_row_ids"] == upstream_row["package_variant_row_ids"], f"{platform_id} package variant rows drifted")
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
        expect(row["issue_ref"] == upstream_row["issue_ref"], f"{platform_id} issue_ref drifted from upstream unsupported evidence")
        expected_issue = EXPECTED_UNSUPPORTED_PLATFORM_ISSUES.get(platform_id)
        expect(expected_issue is not None and row["issue_ref"] == expected_issue, f"{platform_id} unsupported issue_ref drifted")
        expect(row["failure_id"] == upstream_row["failure_id"], f"{platform_id} failure_id drifted from upstream unsupported evidence")
        expect(row["failure_id"] in failure_ids, f"{platform_id} failure_id not in unsupported host policy")
        expect(row["fail_closed_evidence_id"] == upstream_row["evidence"]["fail_closed"], f"{platform_id} fail-closed evidence drifted")
        expect(row["package_variant_row_ids"] == upstream_row["package_variant_row_ids"], f"{platform_id} package variant rows drifted")
        _require_policy_record(records_by_id, row["fail_closed_evidence_id"])
        _require_public_commands(row["public_replay_commands"])
    return unsupported_ids


def _validate_package_variant_rows(inputs: ValidationInputs, records_by_id: dict[str, dict[str, Any]], supported_ids: set[str]) -> list[str]:
    upstream_rows = _upstream_package_rows_by_id(inputs.platform_evidence)
    row_ids: list[str] = []
    for row in inputs.source_truth["package_variant_rows"]:
        row_id = str(row["row_id"])
        row_ids.append(row_id)
        expect(row_id in upstream_rows, f"{row_id} is not in upstream package variant evidence")
        upstream = upstream_rows[row_id]
        for field_name in (
            "issue_ref",
            "variant_kind",
            "target_platform_id",
            "platform_ids",
            "package_id",
            "runtime_library_contract",
            "metadata_freshness_guard",
            "artifact_identity_contract",
            "promotion_gate_contract",
            "claim_state",
            "required_evidence_classes",
            "required_missing_evidence_classes",
            "evidence_ids",
            "unsupported_behavior",
            "diagnostic",
        ):
            expect(row[field_name] == upstream[field_name], f"{row_id} {field_name} drifted from upstream package variant evidence")

        platform_ids = {str(platform_id) for platform_id in row.get("platform_ids", [])}
        if row["claim_state"] == "evidence-bound":
            expect(platform_ids <= supported_ids, f"{row_id} widened package support: {sorted(platform_ids - supported_ids)}")
            for evidence_id in row["evidence_ids"]:
                record = records_by_id[str(evidence_id)]
                expect(record.get("claim_weight") == "supporting", f"{row_id} evidence {evidence_id} is not supporting")
            continue
        expect(not platform_ids, f"{row_id} non-supported package row cannot list supported platform ids")
        for evidence_id in row["evidence_ids"]:
            _require_policy_record(records_by_id, str(evidence_id))
    expect(sorted(row_ids) == sorted(upstream_rows), "source-truth package variant rows drifted from upstream evidence")
    return row_ids


def _validate_sanitizer_variant_rows(inputs: ValidationInputs, records_by_id: dict[str, dict[str, Any]], package_row_ids: set[str]) -> list[str]:
    upstream_rows = _upstream_sanitizer_rows_by_id(inputs.platform_evidence)
    variant_ids: list[str] = []
    for row in inputs.source_truth["sanitizer_variant_rows"]:
        variant_id = str(row["variant_id"])
        variant_ids.append(variant_id)
        expect(variant_id in upstream_rows, f"{variant_id} is not in upstream sanitizer evidence")
        upstream = upstream_rows[variant_id]
        for field_name in (
            "issue_ref",
            "sanitizer",
            "claim_state",
            "platform_ids",
            "package_variant_row_id",
            "package_id",
            "runtime_requirement",
            "metadata_freshness_guard",
            "install_guard",
            "package_runtime_contract",
            "required_promotion_evidence",
            "required_missing_evidence_classes",
            "diagnostic",
        ):
            expect(row[field_name] == upstream[field_name], f"{variant_id} {field_name} drifted from upstream sanitizer evidence")
        expect(row["issue_ref"] == EXPECTED_SANITIZER_ISSUES[str(row["sanitizer"])], f"{variant_id} sanitizer issue_ref drifted")
        expect(row["package_variant_row_id"] in package_row_ids, f"{variant_id} package variant row missing from source truth")
        platform_ids = {str(platform_id) for platform_id in row.get("platform_ids", [])}
        if row["claim_state"] == "evidence-bound":
            expect(platform_ids == {"windows-x64"}, f"{variant_id} evidence-bound sanitizer must be windows-x64-only")
            expect(not row["required_missing_evidence_classes"], f"{variant_id} evidence-bound sanitizer listed missing evidence")
            for evidence_id in upstream.get("evidence_ids", []):
                record = records_by_id[str(evidence_id)]
                expect(record.get("claim_weight") == "supporting", f"{variant_id} evidence {evidence_id} is not supporting")
            continue
        expect(row["claim_state"] == "reserved", f"{variant_id} used unknown sanitizer claim state")
        expect(not platform_ids, f"{variant_id} reserved sanitizer cannot list supported platform ids")
        for evidence_id in upstream.get("evidence_ids", []):
            _require_policy_record(records_by_id, str(evidence_id))
    expect(sorted(variant_ids) == sorted(upstream_rows), "source-truth sanitizer rows drifted from upstream evidence")
    return variant_ids


def _validate_umbrella_readiness(
    inputs: ValidationInputs,
    *,
    supported_ids: set[str],
    unsupported_ids: set[str],
    package_variant_row_ids: set[str],
    sanitizer_variant_ids: set[str],
) -> dict[str, Any]:
    contract = inputs.source_truth["umbrella_readiness_contract"]
    expect(contract["umbrella_issue_ref"] == 8206, "platform umbrella issue_ref drifted")
    expect(contract["closure_state"] == "source-owned-fail-closed-ready", "platform umbrella closure state drifted")
    _validate_support_claim_boundary(
        str(contract["support_claim_boundary"]),
        supported_ids=supported_ids,
        unsupported_ids=unsupported_ids,
    )
    expect(
        contract["supported_platform_row_ids"]
        == [row["row_id"] for row in inputs.source_truth["supported_rows"]],
        "platform umbrella supported row ids drifted from source truth",
    )
    expect(
        {"objc3c.package.sanitizer.asan.reserved", "objc3c.package.sanitizer.ubsan.reserved"} <= package_variant_row_ids,
        "platform umbrella sanitizer package rows drifted from source truth",
    )
    expect(
        {"objc3c.toolchain.sanitizer.address", "objc3c.toolchain.sanitizer.undefined"} == sanitizer_variant_ids,
        "platform umbrella sanitizer variant ids drifted",
    )
    hard_fail_ids = {
        str(failure_class.get("failure_id"))
        for failure_class in inputs.unsupported_host_policy.get("hard_fail_classes", [])
        if isinstance(failure_class, dict) and failure_class.get("failure_id")
    }
    expect(
        "native-object-emission-unavailable" in hard_fail_ids,
        "platform umbrella missing native object emission hard-fail policy",
    )

    child_contracts = {
        int(row["issue_ref"]): row
        for row in contract["child_issue_contracts"]
    }
    expected_child_contracts = {
        **_expected_platform_child_issue_contracts(inputs),
        **EXPECTED_FIXED_UMBRELLA_CHILD_ISSUE_CONTRACTS,
    }
    expect(
        set(child_contracts) == set(expected_child_contracts),
        "platform umbrella child issue contracts drifted",
    )
    for issue_ref, expected in expected_child_contracts.items():
        row = child_contracts[issue_ref]
        for field_name in ("row_id", "contract_kind", "claim_state"):
            expect(row[field_name] == expected[field_name], f"platform umbrella child {issue_ref} {field_name} drifted")
        expect(
            tuple(row["required_promotion_evidence"]) == expected["required_promotion_evidence"],
            f"platform umbrella child {issue_ref} promotion evidence drifted",
        )

    upstream_native_contract = inputs.platform_evidence["llvm_version_support_matrix"]["native_object_emission_contract"]
    native_contract = contract["native_object_emission_contract"]
    for field_name in (
        "contract_id",
        "issue_ref",
        "required_tool",
        "required_probe",
        "required_target_probe",
        "success_status",
        "missing_llc_status",
        "missing_filetype_status",
        "target_object_status",
        "mixed_toolchain_status",
        "mismatched_version_status",
        "unsupported_version_status",
        "unresolved_version_status",
        "hosted_runner_behavior",
        "task_hygiene_behavior",
        "conformance_minima_behavior",
        "required_conformance_minima_env",
        "fallback_policy",
        "coherent_toolchain_policy",
    ):
        expect(
            native_contract[field_name] == upstream_native_contract[field_name],
            f"platform umbrella native object emission {field_name} drifted from upstream evidence",
        )
    expect(
        native_contract["fallback_policy"] == "no-clang-fallback-success-claim",
        "platform umbrella allowed a clang substitute object-emission claim",
    )
    expect(
        native_contract["coherent_toolchain_policy"]
        == "no-mixed-root-or-mismatched-version-success-claim",
        "platform umbrella allowed mixed LLVM root or mismatched-version support claims",
    )
    forbidden_overclaims = {str(item) for item in contract["forbidden_overclaims"]}
    expect(
        "native object emission success when llc is missing" in forbidden_overclaims,
        "platform umbrella missing hosted-runner missing-llc overclaim guard",
    )
    expect(
        "clang substitute published as llvm-direct object emission success" in forbidden_overclaims,
        "platform umbrella missing no-clang-substitute overclaim guard",
    )
    _validate_lead_projection_rule(
        str(contract["lead_projection_rule"]),
        supported_ids=supported_ids,
        unsupported_ids=unsupported_ids,
    )
    return {
        "umbrella_issue_ref": contract["umbrella_issue_ref"],
        "closure_state": contract["closure_state"],
        "support_claim_boundary": contract["support_claim_boundary"],
        "child_issue_refs": sorted(child_contracts),
        "native_object_emission_statuses": [
            native_contract["success_status"],
            native_contract["missing_llc_status"],
            native_contract["missing_filetype_status"],
            native_contract["target_object_status"],
            native_contract["mixed_toolchain_status"],
            native_contract["mismatched_version_status"],
            native_contract["unsupported_version_status"],
            native_contract["unresolved_version_status"],
        ],
    }


def _validate_support_claim_boundary(
    boundary: str,
    *,
    supported_ids: set[str],
    unsupported_ids: set[str],
) -> None:
    if boundary == "windows-x64-only":
        expect(supported_ids == {"windows-x64"}, "windows-x64-only boundary drifted from supported rows")
        expect(
            unsupported_ids == set(EXPECTED_UNSUPPORTED_PLATFORM_ISSUES),
            "windows-x64-only boundary drifted from unsupported platform rows",
        )
        return
    prefix = "source-owned:"
    expect(boundary.startswith(prefix), "platform umbrella support boundary used an unknown format")
    boundary_ids = {
        item.strip()
        for item in boundary[len(prefix) :].split(",")
        if item.strip()
    }
    expect(boundary_ids == supported_ids, "platform umbrella support boundary drifted from supported rows")
    expect(
        not boundary_ids.intersection(unsupported_ids),
        "platform umbrella support boundary includes unsupported rows",
    )


def _expected_platform_child_issue_contracts(inputs: ValidationInputs) -> dict[int, dict[str, Any]]:
    expected: dict[int, dict[str, Any]] = {}
    for row in inputs.source_truth["supported_rows"]:
        issue_ref = int(row["issue_ref"])
        if issue_ref not in PLATFORM_HOST_ISSUE_REFS:
            continue
        expected[issue_ref] = {
            "row_id": row["row_id"],
            "contract_kind": "platform-host",
            "claim_state": "evidence-bound",
            "required_promotion_evidence": tuple(REQUIRED_SUPPORTED_EVIDENCE_CLASSES),
        }
    for row in inputs.source_truth["unsupported_rows"]:
        issue_ref = int(row["issue_ref"])
        if issue_ref not in PLATFORM_HOST_ISSUE_REFS:
            continue
        expected[issue_ref] = {
            "row_id": row["row_id"],
            "contract_kind": "platform-host",
            "claim_state": "unsupported",
            "required_promotion_evidence": tuple(REQUIRED_SUPPORTED_EVIDENCE_CLASSES),
        }
    expect(
        set(expected) == set(PLATFORM_HOST_ISSUE_REFS),
        "platform umbrella host child issue contracts no longer cover Linux and macOS",
    )
    return expected


def _validate_lead_projection_rule(
    rule: str,
    *,
    supported_ids: set[str],
    unsupported_ids: set[str],
) -> None:
    for platform_id in supported_ids:
        expect(platform_id in rule, f"platform umbrella projection rule omits supported platform {platform_id}")
    for platform_id in unsupported_ids:
        expect(platform_id in rule, f"platform umbrella projection rule omits unsupported platform {platform_id}")
    if supported_ids == {"windows-x64"}:
        expect(
            "Only the windows-x64 row may be projected as supported" in rule,
            "platform umbrella projection rule does not pin windows-x64-only support",
        )


def _records_by_id(rows: Iterable[dict[str, Any]], field_name: str) -> dict[str, dict[str, Any]]:
    by_id: dict[str, dict[str, Any]] = {}
    for row in rows:
        row_id = str(row.get(field_name, ""))
        expect(row_id, f"host promotion row missing {field_name}")
        expect(row_id not in by_id, f"duplicate host promotion row {row_id}")
        by_id[row_id] = row
    return by_id


def _validate_host_promotion_architecture(
    inputs: ValidationInputs,
    *,
    supported_ids: set[str],
    unsupported_ids: set[str],
) -> dict[str, Any]:
    architecture = inputs.source_truth["host_promotion_architecture"]
    expect(
        architecture["contract_id"] == "objc3c.platform.support.host-promotion.source-truth.v1",
        "host promotion architecture contract_id drifted",
    )
    expect(
        architecture["promotion_policy"] == "real-host-execution-required",
        "host promotion architecture policy drifted",
    )
    expect(
        set(architecture["source_evidence_sections"]) == set(REQUIRED_HOST_PROMOTION_SOURCE_SECTIONS),
        "host promotion source evidence sections drifted",
    )
    expect(
        set(architecture["supported_platform_ids"]) == supported_ids,
        "host promotion supported platform ids drifted",
    )
    expect(
        set(architecture["unsupported_platform_ids"]) == unsupported_ids,
        "host promotion unsupported platform ids drifted",
    )

    host_identities = _records_by_id(inputs.platform_evidence["host_identity_records"], "record_id")
    toolchain_probes = _records_by_id(inputs.platform_evidence["toolchain_probe_records"], "record_id")
    package_roots = _records_by_id(inputs.platform_evidence["package_root_evidence_records"], "record_id")
    native_execution = _records_by_id(inputs.platform_evidence["native_execution_evidence_records"], "record_id")
    object_identities = _records_by_id(inputs.platform_evidence["object_identity_records"], "record_id")
    debug_identities = _records_by_id(inputs.platform_evidence["debug_identity_records"], "record_id")
    package_install_identities = _records_by_id(
        inputs.platform_evidence["package_install_identity_records"],
        "record_id",
    )
    runtime_load_link_proofs = _records_by_id(
        inputs.platform_evidence["runtime_load_link_proof_records"],
        "record_id",
    )
    negative_cases = _records_by_id(inputs.platform_evidence["negative_host_toolchain_cases"], "case_id")
    evidence_records = _evidence_by_id(inputs.platform_evidence)

    expect(
        set(architecture["host_identity_record_ids"]) == set(host_identities),
        "host identity record ids drifted from upstream evidence",
    )
    expect(
        set(architecture["toolchain_probe_record_ids"]) == set(toolchain_probes),
        "toolchain probe record ids drifted from upstream evidence",
    )
    expect(
        set(architecture["package_root_record_ids"]) == set(package_roots),
        "package root record ids drifted from upstream evidence",
    )
    expect(
        set(architecture["native_execution_record_ids"]) == set(native_execution),
        "native execution record ids drifted from upstream evidence",
    )
    expect(
        set(architecture["object_identity_record_ids"]) == set(object_identities),
        "object identity record ids drifted from upstream evidence",
    )
    expect(
        set(architecture["debug_identity_record_ids"]) == set(debug_identities),
        "debug identity record ids drifted from upstream evidence",
    )
    expect(
        set(architecture["package_install_identity_record_ids"]) == set(package_install_identities),
        "package/install identity record ids drifted from upstream evidence",
    )
    expect(
        set(architecture["runtime_load_link_proof_record_ids"]) == set(runtime_load_link_proofs),
        "runtime load/link proof record ids drifted from upstream evidence",
    )
    expect(
        set(REQUIRED_NEGATIVE_HOST_TOOLCHAIN_CASES) <= set(architecture["negative_host_toolchain_case_ids"]),
        "host promotion architecture missing required negative host/toolchain cases",
    )
    expect(
        set(architecture["negative_host_toolchain_case_ids"]) <= set(negative_cases),
        "host promotion architecture referenced missing negative host/toolchain cases",
    )
    _validate_hosted_evidence_ingestion(
        architecture,
        upstream=inputs.platform_evidence["host_evidence_contract"],
        records_by_id=evidence_records,
    )

    unsupported_execution = {
        str(record["platform_id"]): record
        for record in native_execution.values()
        if str(record.get("platform_id")) in unsupported_ids
    }
    expect(
        set(unsupported_execution) == unsupported_ids,
        "unsupported platform native execution records drifted",
    )
    for platform_id, record in unsupported_execution.items():
        expect(
            record["claim_state"] == "missing-host-execution",
            f"{platform_id} native execution record must remain missing-host-execution",
        )
        expect(record["promotion_allowed"] is False, f"{platform_id} native execution record allowed promotion")
        expect(not record["execution_evidence_ids"], f"{platform_id} native execution record carried execution evidence")

    reviewed_source_sections = {
        "object identity": object_identities,
        "debug identity": debug_identities,
        "package/install identity": package_install_identities,
        "runtime load/link proof": runtime_load_link_proofs,
    }
    for section_name, records in reviewed_source_sections.items():
        unsupported_records = {
            str(record["platform_id"]): record
            for record in records.values()
            if str(record.get("platform_id")) in unsupported_ids
        }
        expect(
            set(unsupported_records) == unsupported_ids,
            f"unsupported platform {section_name} records drifted",
        )
        for platform_id, record in unsupported_records.items():
            expect(
                record["claim_state"] == "fail-closed",
                f"{platform_id} {section_name} record must remain fail-closed",
            )
            expect(
                record["promotion_allowed"] is False,
                f"{platform_id} {section_name} record allowed promotion",
            )
            expect(
                not record["platform_ids"],
                f"{platform_id} {section_name} record widened support",
            )
            expect(
                record["generated_report_support_truth"] is False,
                f"{platform_id} {section_name} generated report became source truth",
            )

    return {
        "contract_id": architecture["contract_id"],
        "promotion_policy": architecture["promotion_policy"],
        "host_identity_record_count": len(host_identities),
        "toolchain_probe_record_count": len(toolchain_probes),
        "package_root_record_count": len(package_roots),
        "native_execution_record_count": len(native_execution),
        "object_identity_record_count": len(object_identities),
        "debug_identity_record_count": len(debug_identities),
        "package_install_identity_record_count": len(package_install_identities),
        "runtime_load_link_proof_record_count": len(runtime_load_link_proofs),
        "negative_host_toolchain_case_count": len(negative_cases),
        "hosted_evidence_ingestion_action": architecture["hosted_evidence_ingestion"]["ingestion_action"],
        "hosted_evidence_candidate_record_count": len(
            architecture["hosted_evidence_ingestion"]["candidate_evidence_record_ids"]
        ),
    }


def _validate_hosted_evidence_ingestion(
    architecture: dict[str, Any],
    *,
    upstream: dict[str, Any],
    records_by_id: dict[str, dict[str, Any]],
) -> None:
    ingestion = architecture["hosted_evidence_ingestion"]
    upstream_ingestion = upstream.get("hosted_evidence_ingestion")
    expect(isinstance(upstream_ingestion, dict), "upstream host evidence contract missing ingestion rules")
    expect(ingestion == upstream_ingestion, "host promotion ingestion rules drifted from upstream evidence")
    expect(ingestion["workflow_path"] == HOST_EVIDENCE_WORKFLOW_PATH, "host evidence workflow path drifted")
    accepted_paths = tuple(str(path).replace("\\", "/") for path in ingestion["accepted_workflow_paths"])
    dispatch_paths = tuple(str(path).replace("\\", "/") for path in ingestion["dispatch_gateway_workflow_paths"])
    expect(set(accepted_paths) == set(HOST_EVIDENCE_ACCEPTED_WORKFLOW_PATHS), "host evidence accepted workflow paths drifted")
    expect(set(dispatch_paths) == set(HOST_EVIDENCE_DISPATCH_GATEWAY_WORKFLOW_PATHS), "host evidence dispatch gateway workflow paths drifted")
    expect(HOST_EVIDENCE_WORKFLOW_PATH in accepted_paths, "host evidence canonical workflow missing from accepted paths")
    for workflow_path in accepted_paths:
        expect(resolve_repo_path(workflow_path).is_file(), f"host evidence workflow file is missing: {workflow_path}")
    expect(ingestion["runner_labels"] == HOST_EVIDENCE_RUNNER_LABELS, "host evidence runner labels drifted")
    expect(ingestion["ingestion_action"] == HOST_EVIDENCE_INGESTION_ACTION, "host evidence ingestion action drifted")
    expect(ingestion["review_action"] == HOST_EVIDENCE_REVIEW_ACTION, "host evidence review action drifted")
    expect(HOST_EVIDENCE_INGESTION_ACTION in ACTION_SPECS, "host evidence ingestion action missing from ACTION_SPECS")
    expect(HOST_EVIDENCE_INGESTION_ACTION in ACTION_HANDLERS, "host evidence ingestion action missing from ACTION_HANDLERS")
    expect(HOST_EVIDENCE_REVIEW_ACTION in ACTION_SPECS, "host evidence review action missing from ACTION_SPECS")
    expect(HOST_EVIDENCE_REVIEW_ACTION in ACTION_HANDLERS, "host evidence review action missing from ACTION_HANDLERS")
    expect(ingestion["ingestion_helper"] == HOST_EVIDENCE_INGESTION_HELPER, "host evidence ingestion helper drifted")
    expect(ingestion["review_helper"] == HOST_EVIDENCE_REVIEW_HELPER, "host evidence review helper drifted")
    expect(resolve_repo_path(HOST_EVIDENCE_INGESTION_HELPER).is_file(), "host evidence ingestion helper is missing")
    expect(resolve_repo_path(HOST_EVIDENCE_REVIEW_HELPER).is_file(), "host evidence review helper is missing")
    expect(ingestion["generated_report_root"] == HOST_EVIDENCE_REPORT_ROOT, "host evidence report root drifted")
    expect(
        ingestion["review_candidate_source_truth_path"]
        == HOST_EVIDENCE_REVIEW_CANDIDATE_SOURCE_TRUTH_PATH_TEMPLATE,
        "host evidence review candidate path drifted",
    )
    expect(ingestion["generated_only_result"] == "refuse-source-truth-promotion", "generated-only ingestion result drifted")
    expect(ingestion["review_promotion_policy"] == "checked-in-source-truth-required", "review promotion policy drifted")
    expect(ingestion["reviewed_source_truth_required"] is True, "host evidence review requirement drifted")
    expect(
        ingestion["support_rows_remain_fail_closed_until_reviewed"] is True,
        "host evidence fail-closed review boundary drifted",
    )
    candidate_ids = tuple(str(record_id) for record_id in ingestion["candidate_evidence_record_ids"])
    expect(set(candidate_ids) == set(HOST_EVIDENCE_CANDIDATE_RECORD_IDS), "host evidence candidate record ids drifted")
    for record_id in candidate_ids:
        record = _require_policy_record(records_by_id, record_id)
        expect(record["evidence_class"] == "hosted_ci", f"{record_id} must remain hosted_ci evidence")
        expect(not record["supports_platform_ids"], f"{record_id} generated host evidence widened support")
        source_paths = {str(path).replace("\\", "/") for path in record["source_paths"]}
        for workflow_path in accepted_paths:
            expect(workflow_path in source_paths, f"{record_id} missing workflow source path: {workflow_path}")
        expect(HOST_EVIDENCE_INGESTION_HELPER in source_paths, f"{record_id} missing ingestion helper source path")
        expect(HOST_EVIDENCE_REVIEW_HELPER in source_paths, f"{record_id} missing review helper source path")
        replay_commands = [str(command) for command in record.get("replay_commands", [])]
        expect(
            any(f"npm run objc3c -- {HOST_EVIDENCE_INGESTION_ACTION}" in command for command in replay_commands),
            f"{record_id} missing public ingestion command",
        )
        expect(
            any(f"npm run objc3c -- {HOST_EVIDENCE_REVIEW_ACTION}" in command for command in replay_commands),
            f"{record_id} missing public reviewed-source staging command",
        )
        generated_paths = [str(path).replace("\\", "/") for path in record["generated_report_paths"]]
        platform_id = record_id.removeprefix(
            "objc3c.evidence.hosted-ci."
        ).removesuffix(".generated-host-run")
        expect(
            host_evidence_review_candidate_path_for_platform(platform_id)
            in generated_paths,
            f"{record_id} missing review candidate generated path",
        )
        expect(
            any(path.startswith(f"{HOST_EVIDENCE_REPORT_ROOT}/") for path in generated_paths),
            f"{record_id} missing platform host evidence report path",
        )


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
    issue_refs = {int(issue_ref) for issue_ref in inputs.source_truth.get("roadmap_issue_refs", [])}
    expect(issue_refs == set(REQUIRED_ROADMAP_ISSUE_REFS), "source truth roadmap issue refs drifted")
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
    package_variant_row_ids = _validate_package_variant_rows(inputs, records_by_id, supported_ids)
    sanitizer_variant_ids = _validate_sanitizer_variant_rows(inputs, records_by_id, set(package_variant_row_ids))
    umbrella_readiness = _validate_umbrella_readiness(
        inputs,
        supported_ids=supported_ids,
        unsupported_ids=set(unsupported_ids),
        package_variant_row_ids=set(package_variant_row_ids),
        sanitizer_variant_ids=set(sanitizer_variant_ids),
    )
    host_promotion_architecture = _validate_host_promotion_architecture(
        inputs,
        supported_ids=supported_ids,
        unsupported_ids=set(unsupported_ids),
    )

    return {
        "contract_id": "objc3c.platform.support.source_truth.validation.summary.v1",
        "status": "PASS",
        "source_path": repo_rel(source_truth_path) if source_truth_path.resolve().is_relative_to(ROOT.resolve()) else source_truth_path.as_posix(),
        "schema_path": inputs.source_truth["schema_path"],
        "issue": inputs.source_truth["issue"],
        "supported_platform_ids": sorted(supported_ids),
        "unsupported_platform_ids": sorted(unsupported_ids),
        "roadmap_issue_refs": list(REQUIRED_ROADMAP_ISSUE_REFS),
        "required_supported_evidence_classes": list(REQUIRED_SUPPORTED_EVIDENCE_CLASSES),
        "required_auxiliary_evidence_classes": list(REQUIRED_AUXILIARY_EVIDENCE_CLASSES),
        "required_toolchain_components": list(REQUIRED_TOOLCHAIN_COMPONENTS),
        "validated_toolchain_components": sorted(toolchain_components),
        "package_variant_row_ids": sorted(package_variant_row_ids),
        "sanitizer_variant_ids": sorted(sanitizer_variant_ids),
        "umbrella_readiness": umbrella_readiness,
        "host_promotion_architecture": host_promotion_architecture,
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
