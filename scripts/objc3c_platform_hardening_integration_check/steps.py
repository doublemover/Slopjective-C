"""Workflow step execution and step-summary classification."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command, run_completed
from platform_hardening_contracts import (
    BUILD_PACKAGE_VALIDATION_SCRIPT,
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    CHANNEL_CATALOG_PATH,
    INSTALL_MATRIX_INTEGRATION_SCRIPT,
    INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
    PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
    RELEASE_PUBLICATION_SUMMARY_PATH,
    PLATFORM_HARDENING_SUMMARY_BUILDERS,
    ROOT,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    TOOLCHAIN_RANGE_REPLAY_SCRIPT,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    UPDATE_MANIFEST_PATH,
    UPGRADE_SUPPORT_REPORT_PATH,
    summary_passes,
)
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .assertions import expect


STEP_SUMMARY_PATHS = {
    "check-platform-hardening-build-package-validation": BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    "check-platform-hardening-toolchain-range-replay": TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    "check-platform-hardening-install-matrix-integration": INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
}

STEP_REUSE_REQUIRED_PATHS = {
    "check-platform-hardening-build-package-validation": (
        SUPPORT_MATRIX_ARTIFACT_PATH,
        BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
        PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
    ),
    "check-platform-hardening-toolchain-range-replay": (
        SUPPORT_MATRIX_ARTIFACT_PATH,
        TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
        RELEASE_PUBLICATION_SUMMARY_PATH,
        UPDATE_MANIFEST_PATH,
        UPGRADE_SUPPORT_REPORT_PATH,
        CHANNEL_CATALOG_PATH,
    ),
    "check-platform-hardening-install-matrix-integration": (
        SUPPORT_MATRIX_ARTIFACT_PATH,
        BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
        TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
        INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
        PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
    ),
}


def run_step(name: str, command: list[str]) -> dict[str, Any]:
    completed = run_completed(command, cwd=ROOT, capture_output=False)
    return {"name": name, "command": command, "exit_code": completed.returncode}


def can_reuse_pass_summary(name: str) -> bool:
    summary_path = STEP_SUMMARY_PATHS.get(name)
    if summary_path is None or not summary_path.is_file():
        return False
    required_paths = STEP_REUSE_REQUIRED_PATHS.get(name, ())
    if any(not path.is_file() for path in required_paths):
        return False
    return summary_passes(load_json(summary_path))


def run_summary_step(name: str, command: list[str]) -> dict[str, Any]:
    if can_reuse_pass_summary(name):
        return {
            "name": name,
            "command": command,
            "exit_code": 0,
            "reused_summary": True,
            "summary_path": repo_rel(STEP_SUMMARY_PATHS[name]),
        }
    return run_step(name, command)


def run_live_steps() -> list[dict[str, Any]]:
    return [
        run_step("build-platform-support-matrix", public_workflow_command("build-platform-support-matrix")),
        *(
            run_step(f"summary-builder:{repo_rel(builder)}", python_script_command(builder))
            for builder in PLATFORM_HARDENING_SUMMARY_BUILDERS
        ),
        run_summary_step(
            "check-platform-hardening-build-package-validation",
            python_script_command(BUILD_PACKAGE_VALIDATION_SCRIPT),
        ),
        run_summary_step(
            "check-platform-hardening-toolchain-range-replay",
            python_script_command(TOOLCHAIN_RANGE_REPLAY_SCRIPT),
        ),
        run_summary_step(
            "check-platform-hardening-install-matrix-integration",
            python_script_command(INSTALL_MATRIX_INTEGRATION_SCRIPT),
        ),
    ]


def classify_step_results(steps: list[dict[str, Any]], failures: list[str]) -> None:
    for step in steps:
        summary_path = STEP_SUMMARY_PATHS.get(str(step["name"]))
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
