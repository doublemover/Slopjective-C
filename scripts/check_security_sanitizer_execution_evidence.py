#!/usr/bin/env python3
"""Validate the sanitizer native execution evidence contract without promoting support."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import validate_json_schema
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel


SCHEMA_PATH = ROOT / "schemas" / "objc3c-sanitizer-execution-evidence-v1.schema.json"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "security_hardening"
    / "sanitizer_execution_evidence_contract.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "security-hardening"
    / "sanitizer-execution-evidence-summary.json"
)
SOURCE_SURFACE = (
    ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "source_surface.json"
)
WORKFLOW_SURFACE = (
    ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "workflow_surface.json"
)

CONTRACT_ID = "objc3c.security.hardening.sanitizer.execution-evidence.contract.v1"
CONTRACT_REL_PATH = "tests/tooling/fixtures/security_hardening/sanitizer_execution_evidence_contract.json"
SUMMARY_CONTRACT_ID = "objc3c.security.hardening.sanitizer.execution-evidence.summary.v1"
SCHEMA_ID = "https://objc3c.dev/schemas/objc3c-sanitizer-execution-evidence-v1.schema.json"
PUBLIC_ACTION = "check-security-sanitizer-execution-evidence"
REQUIRED_SCHEMA_TOKENS = {
    "public_action",
    "runtime_library_manifest_path",
    "runtime_library_manifest_digest",
    "runtime_library_artifacts",
    "native_execution_command",
    "expected_detection_record",
    "support_truth",
    "trap_or_recover_mode",
    "generated_only_evidence_allowed",
}
REQUIRED_VARIANTS = {
    "objc3c.toolchain.sanitizer.address": {
        "issue_ref": 8230,
        "sanitizer": "address",
        "package_id": "org.objc3c.runtime:objc3c-runtime-asan",
        "package_channel_id": "windows-x64-sanitizer-asan",
        "runtime_library_manifest_path": "share/objc3c/sanitizer/asan-runtime-libraries.json",
        "runtime_artifacts": {
            "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
            "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
            "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib",
        },
        "runtime_library_id": "clang_rt.asan",
        "profile": "asan-native-detection",
        "env_var": "ASAN_OPTIONS",
        "record_kind": "asan-diagnostic",
    },
    "objc3c.toolchain.sanitizer.undefined": {
        "issue_ref": 8231,
        "sanitizer": "undefined",
        "package_id": "org.objc3c.runtime:objc3c-runtime-ubsan",
        "package_channel_id": "windows-x64-sanitizer-ubsan",
        "runtime_library_manifest_path": "share/objc3c/sanitizer/ubsan-runtime-libraries.json",
        "runtime_artifacts": {
            "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
            "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib",
        },
        "runtime_library_id": "clang_rt.ubsan",
        "profile": "ubsan-native-detection",
        "env_var": "UBSAN_OPTIONS",
        "record_kind": "ubsan-trap",
    },
}
REQUIRED_NEGATIVE_KINDS = {
    "missing-runtime-manifest",
    "mixed-release-sanitizer-runtime",
    "unsupported-host",
    "stale-runtime-manifest-digest",
    "generated-only-evidence",
}
REQUIRED_BLOCKS = {"package", "install", "execution", "publication", "support-promotion"}


def fail(message: str) -> int:
    print(f"security-sanitizer-execution-evidence: {message}", file=sys.stderr)
    return 1


def require_object(value: Any, label: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise RuntimeError(f"{label} must be an object")
    return value


def require_string_list(value: Any, label: str) -> list[str]:
    if not isinstance(value, list) or not value:
        raise RuntimeError(f"{label} must be a non-empty list")
    if not all(isinstance(item, str) and item for item in value):
        raise RuntimeError(f"{label} must contain only non-empty strings")
    return value


def require_false(value: Any, label: str) -> None:
    if value is not False:
        raise RuntimeError(f"{label} must remain false until real native execution exists")


def require_source_relative_path(raw_path: str, label: str) -> None:
    path = Path(raw_path)
    if path.is_absolute() or raw_path.startswith(("tmp/", "temp/")):
        raise RuntimeError(f"{label} must be a source-owned repo-relative path")


def validate_schema_surface() -> dict[str, object]:
    schema = load_json(SCHEMA_PATH)
    if schema.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
        raise RuntimeError("sanitizer execution schema draft drifted")
    if schema.get("$id") != SCHEMA_ID:
        raise RuntimeError("sanitizer execution schema id drifted")
    if schema.get("properties", {}).get("contract_id", {}).get("const") != CONTRACT_ID:
        raise RuntimeError("sanitizer execution schema contract id drifted")
    schema_text = SCHEMA_PATH.read_text(encoding="utf-8")
    missing_tokens = sorted(token for token in REQUIRED_SCHEMA_TOKENS if token not in schema_text)
    if missing_tokens:
        raise RuntimeError(f"sanitizer execution schema missing tokens: {missing_tokens}")
    return {
        "schema": repo_rel(SCHEMA_PATH),
        "schema_id": SCHEMA_ID,
        "required_token_count": len(REQUIRED_SCHEMA_TOKENS),
    }


def validate_contract_shape(contract: dict[str, Any]) -> None:
    schema = load_json(SCHEMA_PATH)
    try:
        validate_json_schema(contract, schema)
    except Exception as exc:  # pragma: no cover - exact jsonschema exception type is helper-owned.
        raise RuntimeError(f"contract schema validation failed: {exc}") from exc


def validate_source_truth_policy(contract: dict[str, Any]) -> dict[str, object]:
    require_false(contract.get("support_truth"), "support_truth")
    require_false(contract.get("native_execution_claimed"), "native_execution_claimed")
    require_false(contract.get("support_promotion_allowed"), "support_promotion_allowed")
    policy = require_object(contract.get("source_truth_policy"), "source_truth_policy")
    if policy.get("checked_fixture_is_source_truth") is not True:
        raise RuntimeError("checked fixture must be the source truth")
    require_false(policy.get("generated_reports_are_source_truth"), "generated_reports_are_source_truth")
    require_false(policy.get("generated_only_evidence_allowed"), "generated_only_evidence_allowed")
    if policy.get("promotion_boundary") != "reserved-until-real-native-sanitizer-execution":
        raise RuntimeError("promotion boundary drifted from real native execution requirement")
    return {
        "checked_fixture_is_source_truth": True,
        "generated_reports_are_source_truth": False,
        "generated_only_evidence_allowed": False,
        "promotion_boundary": policy["promotion_boundary"],
    }


def require_action_list(payload: dict[str, Any], list_name: str, action_name: str, label: str) -> None:
    actions = payload.get(list_name)
    if not isinstance(actions, list) or action_name not in actions:
        raise RuntimeError(f"{label}.{list_name} missing {action_name}")


def validate_surface_contract_metadata(
    payload: dict[str, Any],
    label: str,
    action_name: str,
) -> dict[str, object]:
    metadata = require_object(
        payload.get("sanitizer_execution_evidence_contract"),
        f"{label}.sanitizer_execution_evidence_contract",
    )
    expected = {
        "public_action": action_name,
        "source_contract": CONTRACT_REL_PATH,
        "schema": repo_rel(SCHEMA_PATH),
        "checker": "scripts/check_security_sanitizer_execution_evidence.py",
        "support_truth": False,
        "native_execution_claimed": False,
        "support_promotion_allowed": False,
    }
    for key, value in expected.items():
        if metadata.get(key) != value:
            raise RuntimeError(f"{label}.sanitizer_execution_evidence_contract.{key} drifted")
    return expected


def validate_public_action_surfaces(contract: dict[str, Any]) -> dict[str, object]:
    action_name = str(contract.get("public_action", ""))
    if action_name != PUBLIC_ACTION:
        raise RuntimeError(f"public_action must be {PUBLIC_ACTION}")

    source_surface = load_json(SOURCE_SURFACE)
    workflow_surface = load_json(WORKFLOW_SURFACE)
    if source_surface.get("sanitizer_execution_evidence_contract") != CONTRACT_REL_PATH:
        raise RuntimeError("source surface sanitizer execution evidence contract path drifted")

    require_action_list(source_surface, "public_actions", action_name, "source_surface")
    source_owner_policy = require_object(
        source_surface.get("owner_policy"),
        "source_surface.owner_policy",
    )
    require_action_list(
        source_owner_policy,
        "owned_actions",
        action_name,
        "source_surface.owner_policy",
    )

    require_action_list(workflow_surface, "required_actions", action_name, "workflow_surface")
    workflow_owner_policy = require_object(
        workflow_surface.get("owner_policy"),
        "workflow_surface.owner_policy",
    )
    require_action_list(
        workflow_owner_policy,
        "owned_actions",
        action_name,
        "workflow_surface.owner_policy",
    )
    require_action_list(
        workflow_owner_policy,
        "workflow_child_actions",
        action_name,
        "workflow_surface.owner_policy",
    )
    require_action_list(workflow_surface, "validation_child_actions", action_name, "workflow_surface")

    source_metadata = validate_surface_contract_metadata(
        source_owner_policy,
        "source_surface.owner_policy",
        action_name,
    )
    workflow_metadata = validate_surface_contract_metadata(
        workflow_owner_policy,
        "workflow_surface.owner_policy",
        action_name,
    )
    return {
        "public_action": action_name,
        "source_surface": repo_rel(SOURCE_SURFACE),
        "workflow_surface": repo_rel(WORKFLOW_SURFACE),
        "source_contract": CONTRACT_REL_PATH,
        "schema": repo_rel(SCHEMA_PATH),
        "checker": "scripts/check_security_sanitizer_execution_evidence.py",
        "source_surface_metadata": source_metadata,
        "workflow_surface_metadata": workflow_metadata,
    }


def validate_positive_case(case: dict[str, Any]) -> dict[str, object]:
    variant_id = str(case.get("variant_id", ""))
    if variant_id not in REQUIRED_VARIANTS:
        raise RuntimeError(f"unknown sanitizer execution variant {variant_id}")
    expected = REQUIRED_VARIANTS[variant_id]
    for key in ("issue_ref", "sanitizer", "package_id", "package_channel_id", "runtime_library_manifest_path"):
        if case.get(key) != expected[key]:
            raise RuntimeError(f"{variant_id} {key} drifted")
    if case.get("target_platform_id") != "windows-x64":
        raise RuntimeError(f"{variant_id} target platform must be windows-x64 until promoted")
    require_source_relative_path(
        str(case.get("runtime_library_manifest_path", "")),
        f"{variant_id}.runtime_library_manifest_path",
    )
    if not str(case.get("runtime_library_manifest_digest", "")):
        raise RuntimeError(f"{variant_id} missing runtime manifest digest")
    require_false(case.get("support_truth"), f"{variant_id}.support_truth")
    require_false(case.get("native_execution_claimed"), f"{variant_id}.native_execution_claimed")
    require_false(case.get("support_promotion_allowed"), f"{variant_id}.support_promotion_allowed")
    require_false(case.get("generated_only_evidence_allowed"), f"{variant_id}.generated_only_evidence_allowed")

    artifacts = case.get("runtime_library_artifacts")
    if not isinstance(artifacts, list) or not artifacts:
        raise RuntimeError(f"{variant_id} runtime_library_artifacts must be non-empty")
    artifact_paths: set[str] = set()
    for index, artifact_value in enumerate(artifacts):
        artifact = require_object(artifact_value, f"{variant_id}.runtime_library_artifacts[{index}]")
        artifact_path = str(artifact.get("artifact", ""))
        require_source_relative_path(artifact_path, f"{variant_id}.runtime_library_artifacts[{index}].artifact")
        artifact_paths.add(artifact_path)
        if artifact.get("runtime_library_id") != expected["runtime_library_id"]:
            raise RuntimeError(f"{variant_id} runtime library id drifted")
        if not str(artifact.get("sha256", "")):
            raise RuntimeError(f"{variant_id} artifact digest missing")
    if artifact_paths != expected["runtime_artifacts"]:
        raise RuntimeError(f"{variant_id} runtime artifacts drifted")

    command = require_object(case.get("native_execution_command"), f"{variant_id}.native_execution_command")
    command_tokens = require_string_list(command.get("command"), f"{variant_id}.native_execution_command.command")
    if command.get("profile") != expected["profile"] or expected["profile"] not in command_tokens:
        raise RuntimeError(f"{variant_id} native execution profile drifted")
    environment = require_object(command.get("environment"), f"{variant_id}.native_execution_command.environment")
    if expected["env_var"] not in environment:
        raise RuntimeError(f"{variant_id} native execution environment missing {expected['env_var']}")
    if command.get("real_native_run_required") is not True:
        raise RuntimeError(f"{variant_id} must require a real native run")
    require_false(command.get("generated_only_evidence_allowed"), f"{variant_id}.native_execution_command.generated_only_evidence_allowed")

    if expected["sanitizer"] == "undefined":
        if case.get("trap_or_recover_mode") not in {"trap", "recover"}:
            raise RuntimeError(f"{variant_id} UBSan case missing explicit trap/recover mode")
        if str(case["trap_or_recover_mode"]) not in command_tokens:
            raise RuntimeError(f"{variant_id} UBSan command does not carry trap/recover mode")
    elif "trap_or_recover_mode" in case:
        raise RuntimeError(f"{variant_id} ASan case must not carry UBSan trap/recover mode")

    detection = require_object(case.get("expected_detection_record"), f"{variant_id}.expected_detection_record")
    if detection.get("record_kind") != expected["record_kind"]:
        raise RuntimeError(f"{variant_id} detection record kind drifted")
    if detection.get("required_behavior") != "record-only-no-support-promotion":
        raise RuntimeError(f"{variant_id} detection record promoted support")
    require_false(detection.get("support_truth"), f"{variant_id}.expected_detection_record.support_truth")

    for list_name in ("report_refs", "output_refs"):
        for ref in require_string_list(case.get(list_name), f"{variant_id}.{list_name}"):
            if not ref.startswith("tmp/reports/security-hardening/"):
                raise RuntimeError(f"{variant_id}.{list_name} must point at security-hardening reports")

    return {
        "variant_id": variant_id,
        "issue_ref": expected["issue_ref"],
        "sanitizer": expected["sanitizer"],
        "package_id": expected["package_id"],
        "package_channel_id": expected["package_channel_id"],
        "runtime_library_manifest_path": expected["runtime_library_manifest_path"],
        "runtime_artifact_count": len(artifact_paths),
        "native_execution_profile": expected["profile"],
        "support_truth": False,
        "native_execution_claimed": False,
        "support_promotion_allowed": False,
    }


def validate_positive_cases(contract: dict[str, Any]) -> list[dict[str, object]]:
    raw_cases = contract.get("positive_contract_fixtures")
    if not isinstance(raw_cases, list) or not raw_cases:
        raise RuntimeError("positive_contract_fixtures must be non-empty")
    cases = [validate_positive_case(require_object(case, "positive_contract_fixtures[]")) for case in raw_cases]
    seen_variants = {str(case["variant_id"]) for case in cases}
    missing = sorted(set(REQUIRED_VARIANTS) - seen_variants)
    if missing:
        raise RuntimeError(f"missing sanitizer execution positive variants: {missing}")
    return cases


def validate_negative_cases(contract: dict[str, Any]) -> dict[str, object]:
    raw_cases = contract.get("negative_contract_fixtures")
    if not isinstance(raw_cases, list) or not raw_cases:
        raise RuntimeError("negative_contract_fixtures must be non-empty")
    seen_pairs: set[tuple[str, str]] = set()
    for raw_case in raw_cases:
        case = require_object(raw_case, "negative_contract_fixtures[]")
        variant_id = str(case.get("variant_id", ""))
        failure_kind = str(case.get("failure_kind", ""))
        if variant_id not in REQUIRED_VARIANTS:
            raise RuntimeError(f"negative case used unknown variant {variant_id}")
        if failure_kind not in REQUIRED_NEGATIVE_KINDS:
            raise RuntimeError(f"{variant_id} negative case used unknown failure kind {failure_kind}")
        if (variant_id, failure_kind) in seen_pairs:
            raise RuntimeError(f"duplicate negative case for {variant_id} {failure_kind}")
        seen_pairs.add((variant_id, failure_kind))
        if not str(case.get("required_behavior", "")).startswith("fail-closed"):
            raise RuntimeError(f"{variant_id} {failure_kind} does not fail closed")
        blocks = set(require_string_list(case.get("blocks"), f"{variant_id}.{failure_kind}.blocks"))
        if not REQUIRED_BLOCKS <= blocks:
            raise RuntimeError(f"{variant_id} {failure_kind} does not block every promotion surface")
        require_false(case.get("support_truth"), f"{variant_id}.{failure_kind}.support_truth")
        require_false(case.get("native_execution_claimed"), f"{variant_id}.{failure_kind}.native_execution_claimed")
        require_false(case.get("support_promotion_allowed"), f"{variant_id}.{failure_kind}.support_promotion_allowed")

    expected_pairs = {
        (variant_id, failure_kind)
        for variant_id in REQUIRED_VARIANTS
        for failure_kind in REQUIRED_NEGATIVE_KINDS
    }
    missing = sorted(expected_pairs - seen_pairs)
    if missing:
        raise RuntimeError(f"missing sanitizer execution negative cases: {missing}")
    return {
        "negative_case_count": len(raw_cases),
        "failure_kinds": sorted(REQUIRED_NEGATIVE_KINDS),
        "blocks": sorted(REQUIRED_BLOCKS),
    }


def validate_report_contract(contract: dict[str, Any]) -> dict[str, object]:
    report_contract = require_object(contract.get("report_contract"), "report_contract")
    if report_contract.get("required_report_root") != "tmp/reports/security-hardening":
        raise RuntimeError("report root drifted")
    require_false(
        report_contract.get("native_sanitizer_execution_claimed"),
        "report_contract.native_sanitizer_execution_claimed",
    )
    require_false(report_contract.get("support_truth"), "report_contract.support_truth")
    refs = require_string_list(report_contract.get("required_output_refs"), "report_contract.required_output_refs")
    if not all(ref.startswith("tmp/reports/security-hardening/") for ref in refs):
        raise RuntimeError("report output refs must stay under tmp/reports/security-hardening")
    return {
        "required_report_root": report_contract["required_report_root"],
        "required_output_refs": refs,
        "native_sanitizer_execution_claimed": False,
        "support_truth": False,
    }


def main() -> int:
    try:
        schema_surface = validate_schema_surface()
        contract = load_json(CONTRACT_PATH)
        if contract.get("contract_id") != CONTRACT_ID:
            raise RuntimeError("unexpected sanitizer execution evidence contract_id")
        validate_contract_shape(contract)
        public_action_surface = validate_public_action_surfaces(contract)
        source_truth_policy = validate_source_truth_policy(contract)
        positive_cases = validate_positive_cases(contract)
        negative_cases = validate_negative_cases(contract)
        report_contract = validate_report_contract(contract)
    except RuntimeError as exc:
        return fail(str(exc))

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_contract": repo_rel(CONTRACT_PATH),
        "public_action": PUBLIC_ACTION,
        "public_action_surface": public_action_surface,
        "schema_surface": schema_surface,
        "source_truth_policy": source_truth_policy,
        "positive_contract_fixtures": positive_cases,
        "negative_contract_fixtures": negative_cases,
        "report_contract": report_contract,
        "support_truth": False,
        "native_execution_claimed": False,
        "support_promotion_allowed": False,
    }
    write_report_json(SUMMARY_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("security-sanitizer-execution-evidence: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
