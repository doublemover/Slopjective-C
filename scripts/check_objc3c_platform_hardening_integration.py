#!/usr/bin/env python3
"""Validate platform-hardening publication across the live public package and release workflow."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
PACKAGE_MANIFEST = ROOT / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
PACKAGED_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "platform_hardening" / "packaged_smoke_integration_contract.json"
BUILD_PACKAGE_VALIDATION_PY = ROOT / "scripts" / "check_platform_hardening_build_package_validation.py"
TOOLCHAIN_RANGE_REPLAY_PY = ROOT / "scripts" / "check_platform_hardening_toolchain_range_replay.py"
INSTALL_MATRIX_INTEGRATION_PY = ROOT / "scripts" / "check_platform_hardening_install_matrix_integration.py"
SUPPORT_MATRIX_PATH = ROOT / "tmp" / "artifacts" / "platform-hardening" / "objc3c-platform-support-matrix.json"
BUILD_PACKAGE_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "build-package-validation-summary.json"
TOOLCHAIN_RANGE_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "toolchain-range-replay-summary.json"
INSTALL_MATRIX_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "install-matrix-integration-summary.json"
PACKAGE_CHANNELS_SUMMARY = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
RELEASE_OPERATIONS_SUMMARY = ROOT / "tmp" / "reports" / "release-operations" / "publication-summary.json"
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
COMPATIBILITY_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-compatibility-report.json"
CHANNEL_CATALOG = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-release-channel-catalog.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "platform-hardening" / "integration-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def run_step(name: str, command: list[str]) -> dict[str, object]:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
    )
    return {"name": name, "command": command, "exit_code": completed.returncode}


def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    if not (ROOT / "native" / "objc3c" / "src" / "main.cpp").is_file():
        contract = load_json(PACKAGED_CONTRACT)
        manifest = load_json(PACKAGE_MANIFEST)
        failures: list[str] = []
        for field in contract["manifest_fields"]:
            expect(manifest.get(field) not in (None, "", []), f"package manifest missing {field}", failures)
        command_surfaces = manifest.get("command_surfaces", {})
        for command_name in contract["required_command_surfaces"]:
            expect(command_name in command_surfaces, f"package manifest missing command surface {command_name}", failures)
        public_actions = manifest.get("platform_hardening_public_actions", [])
        public_scripts = manifest.get("platform_hardening_public_scripts", [])
        for action in contract["public_actions"]:
            expect(action in public_actions, f"package manifest missing public action {action}", failures)
        for script in contract["public_scripts"]:
            expect(script in public_scripts, f"package manifest missing public script {script}", failures)
        payload = {
            "contract_id": "objc3c.platform.hardening.integration.summary.v1",
            "ok": not failures,
            "failures": failures,
            "mode": "packaged-bundle-smoke",
            "reports": {
                "package_manifest": repo_rel(PACKAGE_MANIFEST),
            },
        }
        SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
        SUMMARY_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
        print(f"summary_path: {repo_rel(SUMMARY_OUT)}")
        if failures:
            print("platform-hardening-integration: FAIL", file=sys.stderr)
            for failure in failures:
                print(f"- {failure}", file=sys.stderr)
            return 1
        print("platform-hardening-integration: PASS")
        return 0

    steps = [
        run_step("build-platform-support-matrix", [sys.executable, str(PUBLIC_RUNNER), "build-platform-support-matrix"]),
        run_step("check-platform-hardening-build-package-validation", [sys.executable, str(BUILD_PACKAGE_VALIDATION_PY)]),
        run_step("check-platform-hardening-toolchain-range-replay", [sys.executable, str(TOOLCHAIN_RANGE_REPLAY_PY)]),
        run_step("check-platform-hardening-install-matrix-integration", [sys.executable, str(INSTALL_MATRIX_INTEGRATION_PY)]),
    ]
    failures: list[str] = []
    step_summary_paths = {
        "check-platform-hardening-build-package-validation": BUILD_PACKAGE_SUMMARY,
        "check-platform-hardening-toolchain-range-replay": TOOLCHAIN_RANGE_SUMMARY,
        "check-platform-hardening-install-matrix-integration": INSTALL_MATRIX_SUMMARY,
    }
    for step in steps:
        summary_path = step_summary_paths.get(str(step["name"]))
        if summary_path is not None and summary_path.is_file():
            summary_payload = load_json(summary_path)
            step["summary_path"] = repo_rel(summary_path)
            step["summary_ok"] = summary_passes(summary_payload)
            step["summary_status"] = summary_payload.get("status", summary_payload.get("ok"))
        else:
            step["summary_ok"] = None
        if step["name"] == "build-platform-support-matrix":
            expect(step["exit_code"] == 0, f"{step['name']} failed", failures)
            continue
        if step["summary_ok"] is True:
            continue
        expect(step["exit_code"] == 0, f"{step['name']} failed", failures)

    required_paths = (
        SUPPORT_MATRIX_PATH,
        BUILD_PACKAGE_SUMMARY,
        TOOLCHAIN_RANGE_SUMMARY,
        INSTALL_MATRIX_SUMMARY,
        PACKAGE_CHANNELS_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        UPDATE_MANIFEST,
        COMPATIBILITY_REPORT,
        CHANNEL_CATALOG,
    )
    for path in required_paths:
        expect(path.is_file(), f"missing expected report: {repo_rel(path)}", failures)

    support_matrix = load_json(SUPPORT_MATRIX_PATH) if SUPPORT_MATRIX_PATH.is_file() else {}
    build_package_summary = load_json(BUILD_PACKAGE_SUMMARY) if BUILD_PACKAGE_SUMMARY.is_file() else {}
    toolchain_range_summary = load_json(TOOLCHAIN_RANGE_SUMMARY) if TOOLCHAIN_RANGE_SUMMARY.is_file() else {}
    install_matrix_summary = load_json(INSTALL_MATRIX_SUMMARY) if INSTALL_MATRIX_SUMMARY.is_file() else {}
    package_channels_summary = load_json(PACKAGE_CHANNELS_SUMMARY) if PACKAGE_CHANNELS_SUMMARY.is_file() else {}
    update_manifest = load_json(UPDATE_MANIFEST) if UPDATE_MANIFEST.is_file() else {}
    compatibility_report = load_json(COMPATIBILITY_REPORT) if COMPATIBILITY_REPORT.is_file() else {}
    channel_catalog = load_json(CHANNEL_CATALOG) if CHANNEL_CATALOG.is_file() else {}

    publication_surface = support_matrix.get("publication_surface", {})
    expected_surface = {
        "inspect_support_matrix_command": "inspect:objc3c:platform-matrix",
        "package_command": "package:objc3c-native:runnable-toolchain",
        "package_channels_command": "package:objc3c:channels",
        "packaging_validation_command": "test:objc3c:packaging-channels",
        "packaging_end_to_end_command": "test:objc3c:packaging-channels:e2e",
        "platform_hardening_validation_command": "test:objc3c:platform-hardening",
        "platform_hardening_end_to_end_command": "test:objc3c:platform-hardening:e2e",
        "release_operations_command": "test:objc3c:release-operations",
        "release_operations_end_to_end_command": "test:objc3c:release-operations:e2e",
    }
    for key, expected in expected_surface.items():
        expect(publication_surface.get(key) == expected, f"support matrix publication surface drifted for {key}", failures)

    expect(summary_passes(build_package_summary), "build/package validation summary did not report PASS", failures)
    expect(summary_passes(toolchain_range_summary), "toolchain-range replay summary did not report PASS", failures)
    expect(summary_passes(install_matrix_summary), "install-matrix integration summary did not report PASS", failures)
    expect(package_channels_summary.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_PATH), "package-channels summary missing platform support matrix link", failures)
    expect(update_manifest.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_PATH), "update manifest missing platform support matrix link", failures)
    expect(update_manifest.get("default_platform_id") == support_matrix.get("default_platform_id"), "update manifest default_platform_id drifted", failures)
    expect(update_manifest.get("supported_platform_ids") == support_matrix.get("claim_boundary", {}).get("supported_platform_ids"), "update manifest supported_platform_ids drifted", failures)
    expect(update_manifest.get("support_tiers") == support_matrix.get("tiers"), "update manifest support tiers drifted", failures)
    expect(compatibility_report.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_PATH), "compatibility report missing platform support matrix link", failures)
    expect(compatibility_report.get("default_platform_id") == support_matrix.get("default_platform_id"), "compatibility report default platform drifted", failures)
    expect(compatibility_report.get("supported_platform_ids") == support_matrix.get("claim_boundary", {}).get("supported_platform_ids"), "compatibility report supported platform ids drifted", failures)
    expect(compatibility_report.get("support_tiers") == support_matrix.get("tiers"), "compatibility report support tiers drifted", failures)
    expect(channel_catalog.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_PATH), "channel catalog missing platform support matrix link", failures)
    expect(channel_catalog.get("default_platform_id") == support_matrix.get("default_platform_id"), "channel catalog default platform drifted", failures)
    expect(channel_catalog.get("supported_platform_ids") == support_matrix.get("claim_boundary", {}).get("supported_platform_ids"), "channel catalog supported platform ids drifted", failures)
    expect(channel_catalog.get("support_tiers") == support_matrix.get("tiers"), "channel catalog support tiers drifted", failures)

    payload = {
        "contract_id": "objc3c.platform.hardening.integration.summary.v1",
        "ok": not failures,
        "failures": failures,
        "steps": steps,
        "reports": {
            "support_matrix": repo_rel(SUPPORT_MATRIX_PATH),
            "build_package_validation": repo_rel(BUILD_PACKAGE_SUMMARY),
            "toolchain_range_replay": repo_rel(TOOLCHAIN_RANGE_SUMMARY),
            "install_matrix_integration": repo_rel(INSTALL_MATRIX_SUMMARY),
            "package_channels": repo_rel(PACKAGE_CHANNELS_SUMMARY),
            "update_manifest": repo_rel(UPDATE_MANIFEST),
            "compatibility_report": repo_rel(COMPATIBILITY_REPORT),
            "channel_catalog": repo_rel(CHANNEL_CATALOG),
        },
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_OUT)}")
    if failures:
        print("platform-hardening-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("platform-hardening-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
