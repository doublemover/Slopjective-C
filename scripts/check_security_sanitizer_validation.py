#!/usr/bin/env python3
"""Validate sanitizer-backed runtime/compiler security hardening coverage."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "security_hardening"
    / "sanitizer_validation_contract.json"
)
SOURCE_SURFACE = (
    ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "source_surface.json"
)
WORKFLOW_SURFACE = (
    ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "workflow_surface.json"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "sanitizer-validation-summary.json"

CONTRACT_ID = "objc3c.security.hardening.sanitizer.validation.contract.v1"
SUMMARY_CONTRACT_ID = "objc3c.security.hardening.sanitizer.validation.summary.v1"
REQUIRED_SANITIZERS = {"ASan", "UBSan"}
REQUIRED_COVERAGE_SURFACES = {"native_runtime", "native_compiler"}
RELEASE_RUNTIME_PACKAGE_IDS = [
    "org.objc3c.runtime:objc3c-runtime-release",
    "org.objc3c.runtime:objc3c-runtime-linux-x64-release",
    "org.objc3c.runtime:objc3c-runtime-darwin-arm64-release",
]
REQUIRED_PACKAGE_VARIANTS = {
    "objc3c.toolchain.sanitizer.address": {
        "issue_ref": 8230,
        "sanitizer": "address",
        "package_variant_row_id": "objc3c.package.sanitizer.asan.reserved",
        "package_id": "org.objc3c.runtime:objc3c-runtime-asan",
        "compiler_flags": {"-fsanitize=address", "-fno-omit-frame-pointer"},
        "linker_flags": {"-fsanitize=address"},
        "runtime_library_ids": ["objc3-runtime", "clang_rt.asan"],
        "required_metadata_fields": {
            "target_platform_id",
            "sanitizer",
            "runtime_library_ids",
            "compiler_flags",
            "linker_flags",
            "environment",
            "release_runtime_package_ids",
            "release_runtime_mixing_allowed",
            "expected_detection_records",
            "unsupported_host_diagnostics",
        },
        "expected_detection_record_ids": {
            "objc3c.sanitizer.address.heap-use-after-free",
            "objc3c.sanitizer.address.container-overflow",
        },
    },
    "objc3c.toolchain.sanitizer.undefined": {
        "issue_ref": 8231,
        "sanitizer": "undefined",
        "package_variant_row_id": "objc3c.package.sanitizer.ubsan.reserved",
        "package_id": "org.objc3c.runtime:objc3c-runtime-ubsan",
        "compiler_flags": {"-fsanitize=undefined", "-fno-omit-frame-pointer"},
        "linker_flags": {"-fsanitize=undefined"},
        "runtime_library_ids": ["objc3-runtime", "clang_rt.ubsan"],
        "required_metadata_fields": {
            "target_platform_id",
            "sanitizer",
            "runtime_library_ids",
            "compiler_flags",
            "linker_flags",
            "trap_or_recover_mode",
            "release_runtime_package_ids",
            "release_runtime_mixing_allowed",
            "expected_detection_records",
            "unsupported_host_diagnostics",
        },
        "expected_detection_record_ids": {
            "objc3c.sanitizer.undefined.signed-integer-overflow",
            "objc3c.sanitizer.undefined.invalid-shift",
        },
    },
}
REQUIRED_PACKAGE_EVIDENCE = {"build", "package", "install", "execution"}
REQUIRED_UNSUPPORTED_DIAGNOSTIC_BLOCKS = {"package", "install", "execution", "publication"}


def fail(message: str) -> int:
    print(f"security-sanitizer-validation: {message}", file=sys.stderr)
    return 1


def require_path(raw_path: str, *, file: bool = True) -> Path:
    path = ROOT / raw_path
    if file and not path.is_file():
        raise RuntimeError(f"missing required file {raw_path}")
    if not file and not path.exists():
        raise RuntimeError(f"missing required path {raw_path}")
    return path


def require_text_tokens(path: Path, tokens: list[str], label: str) -> list[str]:
    text = path.read_text(encoding="utf-8")
    missing = [token for token in tokens if token not in text]
    if missing:
        raise RuntimeError(f"{label} missing tokens in {repo_rel(path)}: {', '.join(missing)}")
    return tokens


def require_action_surfaces(action_name: str) -> None:
    registered_actions = set(public_workflow_action_names())
    if action_name not in registered_actions:
        raise RuntimeError(f"workflow registry missing public action {action_name}")

    source_surface = load_json(SOURCE_SURFACE)
    workflow_surface = load_json(WORKFLOW_SURFACE)
    for payload, list_name in (
        (source_surface, "public_actions"),
        (source_surface.get("owner_policy", {}), "owned_actions"),
        (workflow_surface, "required_actions"),
        (workflow_surface.get("owner_policy", {}), "owned_actions"),
        (workflow_surface, "validation_child_actions"),
        (workflow_surface.get("owner_policy", {}), "workflow_child_actions"),
    ):
        actions = payload.get(list_name) if isinstance(payload, dict) else None
        if not isinstance(actions, list) or action_name not in actions:
            raise RuntimeError(f"{list_name} missing {action_name}")


def require_object(value: Any, label: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise RuntimeError(f"{label} must be an object")
    return value


def require_string_list(value: Any, label: str) -> list[str]:
    if not isinstance(value, list) or not value:
        raise RuntimeError(f"{label} must be a non-empty list")
    return [str(item) for item in value]


def validate_sanitizer_config(contract: dict[str, Any]) -> dict[str, object]:
    config = contract.get("sanitizer_config")
    if not isinstance(config, dict):
        raise RuntimeError("sanitizer_config must be an object")
    config_path = require_path(str(config.get("path", "")))
    checked_tokens: list[str] = []
    for field in ("required_options", "required_compile_flags", "required_link_flags", "required_sanitizer_tokens"):
        tokens = [str(token) for token in config.get(field, [])]
        if not tokens:
            raise RuntimeError(f"sanitizer_config.{field} must be non-empty")
        checked_tokens.extend(require_text_tokens(config_path, tokens, field))
    return {
        "config_path": repo_rel(config_path),
        "checked_token_count": len(checked_tokens),
    }


def validate_target_applications(contract: dict[str, Any]) -> list[dict[str, str]]:
    applications = contract.get("target_applications")
    if not isinstance(applications, list) or not applications:
        raise RuntimeError("target_applications must be a non-empty list")

    checked: list[dict[str, str]] = []
    for entry in applications:
        if not isinstance(entry, dict):
            raise RuntimeError("target_applications entries must be objects")
        raw_path = str(entry.get("path", ""))
        target_token = str(entry.get("target_token", ""))
        surface = str(entry.get("surface", ""))
        owner = str(entry.get("owner", ""))
        if not raw_path or not target_token or not surface or not owner:
            raise RuntimeError("target application entries require path, target_token, surface, and owner")
        path = require_path(raw_path)
        expected_call = f"objc3c_apply_sanitizers({target_token})"
        require_text_tokens(path, [expected_call], f"target application {surface}")
        checked.append(
            {
                "surface": surface,
                "owner": owner,
                "path": repo_rel(path),
                "target_token": target_token,
            }
        )
    return checked


def validate_coverage_matrix(contract: dict[str, Any]) -> list[dict[str, str]]:
    matrix = contract.get("coverage_matrix")
    if not isinstance(matrix, list) or not matrix:
        raise RuntimeError("coverage_matrix must be a non-empty list")

    seen = {
        (str(entry.get("sanitizer")), str(entry.get("surface")))
        for entry in matrix
        if isinstance(entry, dict)
    }
    expected_pairs = {
        (sanitizer, surface)
        for sanitizer in REQUIRED_SANITIZERS
        for surface in REQUIRED_COVERAGE_SURFACES
    }
    missing_pairs = sorted(expected_pairs - seen)
    if missing_pairs:
        raise RuntimeError(f"coverage_matrix missing pairs: {missing_pairs}")

    checked: list[dict[str, str]] = []
    for entry in matrix:
        if not isinstance(entry, dict):
            raise RuntimeError("coverage_matrix entries must be objects")
        if entry.get("coverage") != "config-backed":
            raise RuntimeError("coverage_matrix entries must be config-backed")
        checked.append(
            {
                "sanitizer": str(entry["sanitizer"]),
                "surface": str(entry["surface"]),
                "owner": str(entry["owner"]),
            }
        )
    return checked


def validate_runtime_package_variants(contract: dict[str, Any]) -> list[dict[str, object]]:
    variants = contract.get("runtime_package_variants")
    if not isinstance(variants, list) or not variants:
        raise RuntimeError("runtime_package_variants must be a non-empty list")

    by_id: dict[str, dict[str, Any]] = {}
    for variant in variants:
        if not isinstance(variant, dict):
            raise RuntimeError("runtime_package_variants entries must be objects")
        variant_id = str(variant.get("variant_id", ""))
        if not variant_id:
            raise RuntimeError("runtime package variant missing variant_id")
        if variant_id in by_id:
            raise RuntimeError(f"duplicate runtime package variant {variant_id}")
        by_id[variant_id] = variant

    missing = sorted(set(REQUIRED_PACKAGE_VARIANTS) - set(by_id))
    if missing:
        raise RuntimeError(f"runtime_package_variants missing reserved variants: {missing}")

    checked: list[dict[str, object]] = []
    for variant_id, expected in REQUIRED_PACKAGE_VARIANTS.items():
        variant = by_id[variant_id]
        issue_ref = int(expected["issue_ref"])  # type: ignore[arg-type]
        sanitizer_name = str(expected["sanitizer"])
        package_variant_row_id = str(expected["package_variant_row_id"])
        package_id = str(expected["package_id"])
        if variant.get("issue_ref") != issue_ref:
            raise RuntimeError(f"{variant_id} issue_ref drifted")
        if variant.get("sanitizer") != sanitizer_name:
            raise RuntimeError(f"{variant_id} sanitizer identity drifted")
        if variant.get("package_variant_row_id") != package_variant_row_id:
            raise RuntimeError(f"{variant_id} package variant row identity drifted")
        if variant.get("package_id") != package_id:
            raise RuntimeError(f"{variant_id} package id drifted")
        if variant.get("claim_state") != "reserved":
            raise RuntimeError(f"{variant_id} must remain reserved until package execution evidence exists")
        if variant.get("native_package_execution_claimed") is not False:
            raise RuntimeError(f"{variant_id} must not claim native package execution")
        if variant.get("unsupported_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} package variant does not fail closed")

        build_contract = require_object(variant.get("build_contract"), f"{variant_id}.build_contract")
        compiler_flags = set(require_string_list(build_contract.get("compiler_flags"), f"{variant_id}.build_contract.compiler_flags"))
        linker_flags = set(require_string_list(build_contract.get("linker_flags"), f"{variant_id}.build_contract.linker_flags"))
        require_string_list(build_contract.get("environment_requirements"), f"{variant_id}.build_contract.environment_requirements")
        if not set(expected["compiler_flags"]) <= compiler_flags:  # type: ignore[arg-type]
            raise RuntimeError(f"{variant_id} compiler flags drifted")
        if not set(expected["linker_flags"]) <= linker_flags:  # type: ignore[arg-type]
            raise RuntimeError(f"{variant_id} linker flags drifted")
        if sanitizer_name == "undefined" and build_contract.get("mode") != "explicit-trap-or-recover":
            raise RuntimeError(f"{variant_id} UBSan trap-or-recover mode policy drifted")

        runtime_library_contract = require_object(
            variant.get("runtime_library_contract"),
            f"{variant_id}.runtime_library_contract",
        )
        runtime_library_ids = require_string_list(
            runtime_library_contract.get("runtime_library_ids"),
            f"{variant_id}.runtime_library_contract.runtime_library_ids",
        )
        if runtime_library_ids != expected["runtime_library_ids"]:
            raise RuntimeError(f"{variant_id} runtime library ids drifted")
        if runtime_library_contract.get("missing_runtime_behavior") != "fail-closed-before-package-install":
            raise RuntimeError(f"{variant_id} missing runtime behavior drifted")
        if runtime_library_contract.get("mixed_runtime_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} mixed runtime behavior drifted")

        install_guard = require_object(variant.get("install_guard"), f"{variant_id}.install_guard")
        if "default release runtime" not in str(install_guard.get("release_channel_policy", "")).lower():
            raise RuntimeError(f"{variant_id} release-channel install guard does not name default release runtime isolation")
        if install_guard.get("unsupported_platform_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} unsupported platform behavior drifted")
        if install_guard.get("missing_runtime_behavior") != "fail-closed-before-package-install":
            raise RuntimeError(f"{variant_id} missing runtime install guard drifted")
        if install_guard.get("mixed_runtime_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} mixed runtime install guard drifted")
        if install_guard.get("stale_package_metadata_behavior") != "fail-closed-before-publication":
            raise RuntimeError(f"{variant_id} stale metadata install guard drifted")

        package_runtime_contract = require_object(
            variant.get("package_runtime_contract"),
            f"{variant_id}.package_runtime_contract",
        )
        if package_runtime_contract.get("runtime_probe_required") is not True:
            raise RuntimeError(f"{variant_id} sanitizer runtime probe is not required")
        if package_runtime_contract.get("default_release_channel_allowed") is not False:
            raise RuntimeError(f"{variant_id} sanitizer package leaked into the default release channel")
        if package_runtime_contract.get("report_artifact_support_truth") is not False:
            raise RuntimeError(f"{variant_id} sanitizer reports were treated as support truth")
        if package_runtime_contract.get("mixed_release_sanitizer_runtime_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} mixed release/sanitizer runtime did not fail closed")
        release_runtime_package_ids = require_string_list(
            package_runtime_contract.get("release_runtime_package_ids"),
            f"{variant_id}.package_runtime_contract.release_runtime_package_ids",
        )
        if release_runtime_package_ids != RELEASE_RUNTIME_PACKAGE_IDS:
            raise RuntimeError(f"{variant_id} release runtime package isolation ids drifted")
        if package_id in release_runtime_package_ids:
            raise RuntimeError(f"{variant_id} sanitizer package id matched a release runtime package id")
        if package_runtime_contract.get("release_runtime_mixing_allowed") is not False:
            raise RuntimeError(f"{variant_id} allowed sanitizer/release runtime package mixing")

        detection_records = package_runtime_contract.get("expected_detection_records")
        if not isinstance(detection_records, list) or not detection_records:
            raise RuntimeError(f"{variant_id} missing expected sanitizer detection records")
        detection_record_ids: set[str] = set()
        for record in detection_records:
            record_object = require_object(record, f"{variant_id}.expected_detection_records[]")
            record_id = str(record_object.get("record_id", ""))
            if not record_id:
                raise RuntimeError(f"{variant_id} expected detection record missing record_id")
            detection_record_ids.add(record_id)
            if record_object.get("support_truth") is not False:
                raise RuntimeError(f"{variant_id} detection record was treated as support truth")
            if record_object.get("required_behavior") != "record-only-no-support-promotion":
                raise RuntimeError(f"{variant_id} detection record behavior drifted")
        if detection_record_ids != expected["expected_detection_record_ids"]:
            raise RuntimeError(f"{variant_id} expected detection record ids drifted")

        unsupported_host_diagnostics = package_runtime_contract.get("unsupported_host_diagnostics")
        if not isinstance(unsupported_host_diagnostics, list) or not unsupported_host_diagnostics:
            raise RuntimeError(f"{variant_id} missing unsupported-host diagnostics")
        unsupported_diagnostic_ids: list[str] = []
        for diagnostic in unsupported_host_diagnostics:
            diagnostic_object = require_object(diagnostic, f"{variant_id}.unsupported_host_diagnostics[]")
            diagnostic_id = str(diagnostic_object.get("diagnostic_id", ""))
            if not diagnostic_id:
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic missing diagnostic_id")
            unsupported_diagnostic_ids.append(diagnostic_id)
            if diagnostic_object.get("failure_class") != "unsupported-sanitizer-platform":
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic failure class drifted")
            if diagnostic_object.get("required_behavior") != "fail-closed-before-capability-promotion":
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic behavior drifted")
            blocks = {str(item) for item in diagnostic_object.get("blocks", [])}
            if not REQUIRED_UNSUPPORTED_DIAGNOSTIC_BLOCKS <= blocks:
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic did not block all promotion surfaces")

        required_metadata_fields = {
            str(field)
            for field in package_runtime_contract.get("required_metadata_fields", [])
        }
        if not set(expected["required_metadata_fields"]) <= required_metadata_fields:  # type: ignore[arg-type]
            raise RuntimeError(f"{variant_id} package runtime metadata requirements drifted")
        required_package_evidence = [str(item) for item in variant.get("required_package_evidence", [])]
        if set(required_package_evidence) != REQUIRED_PACKAGE_EVIDENCE:
            raise RuntimeError(f"{variant_id} package variant evidence requirements drifted")
        checked.append(
            {
                "variant_id": variant_id,
                "sanitizer": sanitizer_name,
                "issue_ref": issue_ref,
                "package_variant_row_id": package_variant_row_id,
                "package_id": package_id,
                "claim_state": "reserved",
                "native_package_execution_claimed": False,
                "runtime_probe_required": True,
                "runtime_library_ids": runtime_library_ids,
                "release_runtime_mixing_allowed": False,
                "expected_detection_record_ids": sorted(detection_record_ids),
                "unsupported_host_diagnostic_ids": unsupported_diagnostic_ids,
            }
        )
    return checked


def validate_fixture(contract: dict[str, Any]) -> str:
    fixture = require_path(str(contract.get("source_fixture", "")))
    marker = str(contract.get("fixture_required_marker", ""))
    if not marker:
        raise RuntimeError("fixture_required_marker must be non-empty")
    require_text_tokens(fixture, [marker], "source fixture")
    return repo_rel(fixture)


def validate_report_contract(contract: dict[str, Any]) -> dict[str, object]:
    report_contract = contract.get("report_contract")
    if not isinstance(report_contract, dict):
        raise RuntimeError("report_contract must be an object")
    required_root = str(report_contract.get("required_report_root", ""))
    if required_root != "tmp/reports/security-hardening":
        raise RuntimeError("report_contract required_report_root drifted")
    if report_contract.get("native_sanitizer_execution_claimed") is not False:
        raise RuntimeError("contract must not claim native sanitizer execution")
    summary_path = str(contract.get("summary_path", ""))
    if summary_path != repo_rel(SUMMARY_PATH):
        raise RuntimeError("summary_path drifted from sanitizer validation output")
    return {
        "required_report_root": required_root,
        "native_sanitizer_execution_claimed": False,
    }


def main() -> int:
    try:
        contract = load_json(CONTRACT_PATH)
        if contract.get("contract_id") != CONTRACT_ID:
            raise RuntimeError("unexpected sanitizer validation contract_id")
        action_name = str(contract.get("public_action", ""))
        if not action_name:
            raise RuntimeError("public_action must be non-empty")
        require_action_surfaces(action_name)
        config_summary = validate_sanitizer_config(contract)
        target_applications = validate_target_applications(contract)
        coverage_matrix = validate_coverage_matrix(contract)
        runtime_package_variants = validate_runtime_package_variants(contract)
        fixture_path = validate_fixture(contract)
        report_contract = validate_report_contract(contract)
    except RuntimeError as exc:
        return fail(str(exc))

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_contract": repo_rel(CONTRACT_PATH),
        "public_action": action_name,
        "sanitizer_config": config_summary,
        "target_applications": target_applications,
        "coverage_matrix": coverage_matrix,
        "runtime_package_variants": runtime_package_variants,
        "source_fixture": fixture_path,
        "report_contract": report_contract,
        "owner_boundaries": contract["owner_boundaries"],
        "forbidden_claims": contract["forbidden_claims"],
    }
    write_report_json(SUMMARY_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("security-sanitizer-validation: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
