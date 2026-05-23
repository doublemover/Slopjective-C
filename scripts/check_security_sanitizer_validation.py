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
REQUIRED_PACKAGE_VARIANTS = {
    "objc3c.toolchain.sanitizer.address": 8230,
    "objc3c.toolchain.sanitizer.undefined": 8231,
}


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
    for variant_id, issue_ref in REQUIRED_PACKAGE_VARIANTS.items():
        variant = by_id[variant_id]
        if variant.get("issue_ref") != issue_ref:
            raise RuntimeError(f"{variant_id} issue_ref drifted")
        if variant.get("claim_state") != "reserved":
            raise RuntimeError(f"{variant_id} must remain reserved until package execution evidence exists")
        if variant.get("native_package_execution_claimed") is not False:
            raise RuntimeError(f"{variant_id} must not claim native package execution")
        if variant.get("unsupported_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} package variant does not fail closed")
        required_package_evidence = [str(item) for item in variant.get("required_package_evidence", [])]
        if set(required_package_evidence) != {"build", "package", "install", "execution"}:
            raise RuntimeError(f"{variant_id} package variant evidence requirements drifted")
        checked.append(
            {
                "variant_id": variant_id,
                "issue_ref": issue_ref,
                "package_variant_row_id": str(variant["package_variant_row_id"]),
                "package_id": str(variant["package_id"]),
                "claim_state": "reserved",
                "native_package_execution_claimed": False,
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
