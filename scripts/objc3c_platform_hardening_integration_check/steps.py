"""Workflow step execution and step-summary classification."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command, run_completed
from platform_hardening_contracts import (
    BUILD_PACKAGE_VALIDATION_SCRIPT,
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    INSTALL_MATRIX_INTEGRATION_SCRIPT,
    INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
    PLATFORM_HARDENING_SUMMARY_BUILDERS,
    ROOT,
    TOOLCHAIN_RANGE_REPLAY_SCRIPT,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    summary_passes,
)
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .assertions import expect


STEP_SUMMARY_PATHS = {
    "check-platform-hardening-build-package-validation": BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    "check-platform-hardening-toolchain-range-replay": TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    "check-platform-hardening-install-matrix-integration": INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
}


def run_step(name: str, command: list[str]) -> dict[str, Any]:
    completed = run_completed(command, cwd=ROOT, capture_output=False)
    return {"name": name, "command": command, "exit_code": completed.returncode}


def run_live_steps() -> list[dict[str, Any]]:
    return [
        run_step("build-platform-support-matrix", public_workflow_command("build-platform-support-matrix")),
        *(
            run_step(f"summary-builder:{repo_rel(builder)}", python_script_command(builder))
            for builder in PLATFORM_HARDENING_SUMMARY_BUILDERS
        ),
        run_step("check-platform-hardening-build-package-validation", python_script_command(BUILD_PACKAGE_VALIDATION_SCRIPT)),
        run_step("check-platform-hardening-toolchain-range-replay", python_script_command(TOOLCHAIN_RANGE_REPLAY_SCRIPT)),
        run_step("check-platform-hardening-install-matrix-integration", python_script_command(INSTALL_MATRIX_INTEGRATION_SCRIPT)),
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
