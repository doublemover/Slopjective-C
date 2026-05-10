#!/usr/bin/env python3
"""Validate platform-hardening publication across the live public package and release workflow."""

from __future__ import annotations

import sys

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import python_script_command, run_completed
from platform_hardening_contracts import (
    BUILD_PACKAGE_VALIDATION_SCRIPT,
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    CHANNEL_CATALOG_PATH,
    INSTALL_MATRIX_INTEGRATION_SCRIPT,
    INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
    PACKAGE_CHANNELS_SUMMARY_PATH,
    PACKAGE_MANIFEST_PATH,
    PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH,
    PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH,
    PLATFORM_HARDENING_SUMMARY_BUILDERS,
    PLATFORM_HARDENING_SUMMARY_PATHS,
    RELEASE_PUBLICATION_SUMMARY_PATH,
    ROOT,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    TOOLCHAIN_RANGE_REPLAY_SCRIPT,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    UPGRADE_SUPPORT_REPORT_PATH,
    UPDATE_MANIFEST_PATH,
    platform_hardening_owner_payload,
    summary_passes,
)



def run_step(name: str, command: list[str]) -> dict[str, object]:
    completed = run_completed(command, cwd=ROOT, capture_output=False)
    return {"name": name, "command": command, "exit_code": completed.returncode}


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    if not (ROOT / "native" / "objc3c" / "src" / "main.cpp").is_file():
        contract = load_json(PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH)
        manifest = load_json(PACKAGE_MANIFEST_PATH)
        failures: list[str] = []
        for field in contract["manifest_fields"]:
            expect(manifest.get(field) not in (None, "", []), f"package manifest missing {field}", failures)
        command_surfaces = manifest.get("command_surfaces", {})
        for command_name in contract["required_command_surfaces"]:
            expect(command_name in command_surfaces, f"package manifest missing command surface {command_name}", failures)
        public_actions = manifest.get("platform_hardening_public_actions", [])
        package_bridge = str(contract["package_bridge"])
        manifest_package_bridge = manifest.get("package_bridge")
        for action in contract["public_actions"]:
            expect(action in public_actions, f"package manifest missing public action {action}", failures)
        expect(manifest_package_bridge == package_bridge, f"package manifest missing package bridge {package_bridge}", failures)
        payload = {
            "contract_id": "objc3c.platform.hardening.integration.summary.v1",
            "ok": not failures,
            "owner_policy": platform_hardening_owner_payload(),
            "blocker_metadata": {
                "blocker_owner": "platform-hardening-blockers",
                "blocking_conditions": [
                    "packaged platform hardening manifest missing source contract",
                    "packaged command surface absent from objc3c bridge",
                    "packaged platform hardening validation emitted evidence-log status",
                ],
            },
            "failures": failures,
            "mode": "packaged-bundle-smoke",
            "reports": {
                "package_manifest": repo_rel(PACKAGE_MANIFEST_PATH),
            },
        }
        PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
        write_json_file(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH, payload)
        print(f"summary_path: {repo_rel(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH)}")
        if failures:
            print("platform-hardening-integration: FAIL", file=sys.stderr)
            for failure in failures:
                print(f"- {failure}", file=sys.stderr)
            return 1
        print("platform-hardening-integration: PASS")
        return 0

    steps = [
        run_step("build-platform-support-matrix", public_workflow_command("build-platform-support-matrix")),
        *(
            run_step(f"summary-builder:{repo_rel(builder)}", python_script_command(builder))
            for builder in PLATFORM_HARDENING_SUMMARY_BUILDERS
        ),
        run_step("check-platform-hardening-build-package-validation", python_script_command(BUILD_PACKAGE_VALIDATION_SCRIPT)),
        run_step("check-platform-hardening-toolchain-range-replay", python_script_command(TOOLCHAIN_RANGE_REPLAY_SCRIPT)),
        run_step("check-platform-hardening-install-matrix-integration", python_script_command(INSTALL_MATRIX_INTEGRATION_SCRIPT)),
    ]
    failures: list[str] = []
    step_summary_paths = {
        "check-platform-hardening-build-package-validation": BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
        "check-platform-hardening-toolchain-range-replay": TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
        "check-platform-hardening-install-matrix-integration": INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
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
        SUPPORT_MATRIX_ARTIFACT_PATH,
        *PLATFORM_HARDENING_SUMMARY_PATHS,
        BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
        TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
        INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
        PACKAGE_CHANNELS_SUMMARY_PATH,
        RELEASE_PUBLICATION_SUMMARY_PATH,
        UPDATE_MANIFEST_PATH,
        UPGRADE_SUPPORT_REPORT_PATH,
        CHANNEL_CATALOG_PATH,
    )
    for path in required_paths:
        expect(path.is_file(), f"missing expected report: {repo_rel(path)}", failures)

    support_matrix = load_json(SUPPORT_MATRIX_ARTIFACT_PATH) if SUPPORT_MATRIX_ARTIFACT_PATH.is_file() else {}
    build_package_summary = load_json(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH) if BUILD_PACKAGE_VALIDATION_SUMMARY_PATH.is_file() else {}
    toolchain_range_summary = load_json(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH) if TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH.is_file() else {}
    install_matrix_summary = load_json(INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH) if INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH.is_file() else {}
    package_channels_summary = load_json(PACKAGE_CHANNELS_SUMMARY_PATH) if PACKAGE_CHANNELS_SUMMARY_PATH.is_file() else {}
    update_manifest = load_json(UPDATE_MANIFEST_PATH) if UPDATE_MANIFEST_PATH.is_file() else {}
    upgrade_support_report = load_json(UPGRADE_SUPPORT_REPORT_PATH) if UPGRADE_SUPPORT_REPORT_PATH.is_file() else {}
    channel_catalog = load_json(CHANNEL_CATALOG_PATH) if CHANNEL_CATALOG_PATH.is_file() else {}

    publication_surface = support_matrix.get("publication_surface", {})
    expected_surface = {
        "package_bridge": "objc3c",
        "inspect_support_matrix_command": "build-platform-support-matrix",
        "package_command": "package-runnable-toolchain",
        "package_channels_command": "build-package-channels",
        "packaging_validation_command": "validate-packaging-channels",
        "packaging_end_to_end_command": "validate-packaging-channels-end-to-end",
        "platform_hardening_validation_command": "validate-platform-hardening",
        "platform_hardening_end_to_end_command": "validate-platform-hardening-end-to-end",
        "release_operations_command": "validate-release-operations",
        "release_operations_end_to_end_command": "validate-release-operations-end-to-end",
    }
    for key, expected in expected_surface.items():
        expect(publication_surface.get(key) == expected, f"support matrix publication surface drifted for {key}", failures)

    expect(summary_passes(build_package_summary), "build/package validation summary did not report PASS", failures)
    expect(summary_passes(toolchain_range_summary), "toolchain-range replay summary did not report PASS", failures)
    expect(summary_passes(install_matrix_summary), "install-matrix integration summary did not report PASS", failures)
    expect(package_channels_summary.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH), "package-channels summary missing platform support matrix link", failures)
    expect(update_manifest.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH), "update manifest missing platform support matrix link", failures)
    expect(update_manifest.get("default_platform_id") == support_matrix.get("default_platform_id"), "update manifest default_platform_id drifted", failures)
    expect(update_manifest.get("supported_platform_ids") == support_matrix.get("claim_boundary", {}).get("supported_platform_ids"), "update manifest supported_platform_ids drifted", failures)
    expect(update_manifest.get("support_tiers") == support_matrix.get("tiers"), "update manifest support tiers drifted", failures)
    expect(upgrade_support_report.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH), "upgrade support report missing platform support matrix link", failures)
    expect(upgrade_support_report.get("default_platform_id") == support_matrix.get("default_platform_id"), "upgrade support report default platform drifted", failures)
    expect(upgrade_support_report.get("supported_platform_ids") == support_matrix.get("claim_boundary", {}).get("supported_platform_ids"), "upgrade support report supported platform ids drifted", failures)
    expect(upgrade_support_report.get("support_tiers") == support_matrix.get("tiers"), "upgrade support report support tiers drifted", failures)
    expect(channel_catalog.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH), "channel catalog missing platform support matrix link", failures)
    expect(channel_catalog.get("default_platform_id") == support_matrix.get("default_platform_id"), "channel catalog default platform drifted", failures)
    expect(channel_catalog.get("supported_platform_ids") == support_matrix.get("claim_boundary", {}).get("supported_platform_ids"), "channel catalog supported platform ids drifted", failures)
    expect(channel_catalog.get("support_tiers") == support_matrix.get("tiers"), "channel catalog support tiers drifted", failures)

    payload = {
        "contract_id": "objc3c.platform.hardening.integration.summary.v1",
        "ok": not failures,
        "owner_policy": platform_hardening_owner_payload(),
        "blocker_metadata": {
            "blocker_owner": "platform-hardening-blockers",
            "blocking_conditions": [
                "platform hardening summary builder failed",
                "build/package validation did not pass",
                "toolchain range replay did not pass",
                "install matrix integration did not pass",
                "release publication metadata drifted from platform support matrix",
            ],
        },
        "failures": failures,
        "steps": steps,
        "reports": {
            "support_matrix": repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
            "summary_builders": [repo_rel(path) for path in PLATFORM_HARDENING_SUMMARY_PATHS],
            "build_package_validation": repo_rel(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH),
            "toolchain_range_replay": repo_rel(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH),
            "install_matrix_integration": repo_rel(INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH),
            "package_channels": repo_rel(PACKAGE_CHANNELS_SUMMARY_PATH),
            "update_manifest": repo_rel(UPDATE_MANIFEST_PATH),
            "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT_PATH),
            "channel_catalog": repo_rel(CHANNEL_CATALOG_PATH),
        },
    }
    PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH)}")
    if failures:
        print("platform-hardening-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("platform-hardening-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
