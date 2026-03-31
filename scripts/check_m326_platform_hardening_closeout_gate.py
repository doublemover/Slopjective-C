#!/usr/bin/env python3
"""Run the M326 platform-hardening closeout gate over the live milestone evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "platform-hardening" / "closeout-gate"
JSON_OUT = OUT_DIR / "platform_hardening_closeout_gate.json"
MD_OUT = OUT_DIR / "platform_hardening_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.platform.hardening.closeout.gate.v1"

STEPS = [
    ("build-boundary-inventory-summary", [sys.executable, "scripts/build_platform_hardening_boundary_inventory_summary.py"]),
    ("build-support-tier-policy-summary", [sys.executable, "scripts/build_platform_hardening_support_tier_policy_summary.py"]),
    ("build-unsupported-host-policy-summary", [sys.executable, "scripts/build_platform_hardening_unsupported_host_policy_summary.py"]),
    ("build-toolchain-archive-policy-summary", [sys.executable, "scripts/build_platform_hardening_toolchain_archive_policy_summary.py"]),
    ("build-artifact-contract-summary", [sys.executable, "scripts/build_platform_hardening_artifact_contract_summary.py"]),
    ("render-public-command-surface", [sys.executable, "scripts/render_objc3c_public_command_surface.py"]),
    ("build-public-command-contract", [sys.executable, "scripts/build_objc3c_public_command_contract.py"]),
    ("check-documentation-surface", [sys.executable, "scripts/check_documentation_surface.py"]),
    ("check-repo-superclean-surface", [sys.executable, "scripts/check_repo_superclean_surface.py"]),
]

SUPPORT_MATRIX = ROOT / "tmp" / "artifacts" / "platform-hardening" / "objc3c-platform-support-matrix.json"
BUILD_PACKAGE_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "build-package-validation-summary.json"
TOOLCHAIN_RANGE_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "toolchain-range-replay-summary.json"
INSTALL_MATRIX_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "install-matrix-integration-summary.json"
INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "integration-summary.json"
RUNNABLE_E2E_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "runnable-end-to-end-summary.json"
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
COMPATIBILITY_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-compatibility-report.json"
CHANNEL_CATALOG = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-release-channel-catalog.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def run_step(name: str, command: list[str]) -> dict[str, object]:
    result = subprocess.run(command, cwd=ROOT, check=False, text=True, capture_output=True)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def tier_platform_ids(support_matrix: dict[str, Any], tier_id: str) -> list[str]:
    tiers = support_matrix.get("tiers", [])
    if not isinstance(tiers, list):
        return []
    for tier in tiers:
        if isinstance(tier, dict) and tier.get("tier_id") == tier_id:
            platform_ids = tier.get("platform_ids", [])
            return platform_ids if isinstance(platform_ids, list) else []
    return []


def main() -> int:
    steps: list[dict[str, object]] = []
    failures: list[str] = []
    for name, command in STEPS:
        step = run_step(name, command)
        steps.append(step)
        expect(step["exit_code"] == 0, f"{name} failed", failures)
        if step["exit_code"] != 0:
            break

    required_paths = (
        SUPPORT_MATRIX,
        BUILD_PACKAGE_SUMMARY,
        TOOLCHAIN_RANGE_SUMMARY,
        INSTALL_MATRIX_SUMMARY,
        INTEGRATION_SUMMARY,
        RUNNABLE_E2E_SUMMARY,
        UPDATE_MANIFEST,
        COMPATIBILITY_REPORT,
        CHANNEL_CATALOG,
    )
    for path in required_paths:
        expect(path.is_file(), f"missing expected artifact {repo_rel(path)}", failures)

    support_matrix = load_json(SUPPORT_MATRIX) if SUPPORT_MATRIX.is_file() else {}
    build_package_summary = load_json(BUILD_PACKAGE_SUMMARY) if BUILD_PACKAGE_SUMMARY.is_file() else {}
    toolchain_range_summary = load_json(TOOLCHAIN_RANGE_SUMMARY) if TOOLCHAIN_RANGE_SUMMARY.is_file() else {}
    install_matrix_summary = load_json(INSTALL_MATRIX_SUMMARY) if INSTALL_MATRIX_SUMMARY.is_file() else {}
    integration_summary = load_json(INTEGRATION_SUMMARY) if INTEGRATION_SUMMARY.is_file() else {}
    runnable_e2e_summary = load_json(RUNNABLE_E2E_SUMMARY) if RUNNABLE_E2E_SUMMARY.is_file() else {}
    update_manifest = load_json(UPDATE_MANIFEST) if UPDATE_MANIFEST.is_file() else {}
    compatibility_report = load_json(COMPATIBILITY_REPORT) if COMPATIBILITY_REPORT.is_file() else {}
    channel_catalog = load_json(CHANNEL_CATALOG) if CHANNEL_CATALOG.is_file() else {}

    expected_supported_platform_ids = ["windows-x64"]
    expect(support_matrix.get("default_platform_id") == "windows-x64", "support matrix default platform drifted", failures)
    expect(support_matrix.get("claim_boundary", {}).get("supported_platform_ids") == expected_supported_platform_ids, "support matrix supported platform ids drifted", failures)
    expect(tier_platform_ids(support_matrix, "tier-1") == expected_supported_platform_ids, "tier-1 platform set drifted", failures)
    expect(tier_platform_ids(support_matrix, "tier-2") == [], "tier-2 should remain unpublished", failures)
    expect(tier_platform_ids(support_matrix, "experimental") == [], "experimental tier should remain unpublished", failures)
    expect(build_package_summary.get("status") == "PASS", "build/package summary did not report PASS", failures)
    expect(toolchain_range_summary.get("status") == "PASS", "toolchain-range replay summary did not report PASS", failures)
    expect(install_matrix_summary.get("status") == "PASS", "install-matrix integration summary did not report PASS", failures)
    expect(integration_summary.get("ok") is True, "platform-hardening integration summary did not report ok=true", failures)
    expect(runnable_e2e_summary.get("status") == "PASS", "runnable platform-hardening end-to-end summary did not report PASS", failures)
    expect(update_manifest.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX), "update manifest platform support matrix drifted", failures)
    expect(update_manifest.get("default_platform_id") == "windows-x64", "update manifest default platform drifted", failures)
    expect(update_manifest.get("supported_platform_ids") == expected_supported_platform_ids, "update manifest supported platform ids drifted", failures)
    expect(update_manifest.get("support_tiers") == support_matrix.get("tiers"), "update manifest support tiers drifted", failures)
    expect(compatibility_report.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX), "compatibility report platform support matrix drifted", failures)
    expect(compatibility_report.get("default_platform_id") == "windows-x64", "compatibility report default platform drifted", failures)
    expect(compatibility_report.get("supported_platform_ids") == expected_supported_platform_ids, "compatibility report supported platform ids drifted", failures)
    expect(compatibility_report.get("support_tiers") == support_matrix.get("tiers"), "compatibility report support tiers drifted", failures)
    expect(channel_catalog.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX), "channel catalog platform support matrix drifted", failures)
    expect(channel_catalog.get("default_platform_id") == "windows-x64", "channel catalog default platform drifted", failures)
    expect(channel_catalog.get("supported_platform_ids") == expected_supported_platform_ids, "channel catalog supported platform ids drifted", failures)
    expect(channel_catalog.get("support_tiers") == support_matrix.get("tiers"), "channel catalog support tiers drifted", failures)
    publication_surface = support_matrix.get("publication_surface", {})
    expect(publication_surface.get("inspect_support_matrix_command") == "inspect:objc3c:platform-matrix", "support matrix inspect command drifted", failures)
    expect(publication_surface.get("platform_hardening_validation_command") == "test:objc3c:platform-hardening", "platform hardening validation command drifted", failures)
    expect(publication_surface.get("platform_hardening_end_to_end_command") == "test:objc3c:platform-hardening:e2e", "platform hardening end-to-end command drifted", failures)

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "steps": steps,
        "failures": failures,
        "artifacts": {
            "support_matrix": repo_rel(SUPPORT_MATRIX),
            "build_package_validation": repo_rel(BUILD_PACKAGE_SUMMARY),
            "toolchain_range_replay": repo_rel(TOOLCHAIN_RANGE_SUMMARY),
            "install_matrix_integration": repo_rel(INSTALL_MATRIX_SUMMARY),
            "integration_summary": repo_rel(INTEGRATION_SUMMARY),
            "runnable_end_to_end_summary": repo_rel(RUNNABLE_E2E_SUMMARY),
            "update_manifest": repo_rel(UPDATE_MANIFEST),
            "compatibility_report": repo_rel(COMPATIBILITY_REPORT),
            "channel_catalog": repo_rel(CHANNEL_CATALOG),
        },
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Platform Hardening Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Step count: `{len(steps)}`\n"
        f"- Status: `{payload['status']}`\n"
        f"- Default platform: `windows-x64`\n"
        f"- Supported platform ids: `{', '.join(expected_supported_platform_ids)}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    print("m326-platform-hardening-closeout-gate: PASS" if not failures else "m326-platform-hardening-closeout-gate: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
