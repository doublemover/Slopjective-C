"""Fixture execution and report loading for runtime architecture proof packets."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.subprocesses import python_script_command, run_capture

from .contracts import HARNESS_SCRIPT, HARNESS_SUMMARY_PATH, PUBLIC_SMOKE_SUITE_ID, ROOT


def run_public_smoke_harness() -> None:
    result = run_capture(
        python_script_command(HARNESS_SCRIPT, "--run-suite", PUBLIC_SMOKE_SUITE_ID)
    )
    if result.returncode != 0:
        raise RuntimeError("shared executable acceptance harness failed for public-test-smoke")


def load_harness_summary() -> dict[str, Any]:
    return load_json(HARNESS_SUMMARY_PATH)


def load_public_workflow_report(suite_summary: dict[str, Any]) -> tuple[Path, dict[str, Any]]:
    public_workflow_report_path = ROOT / str(suite_summary.get("report_path", ""))
    return public_workflow_report_path, load_json(public_workflow_report_path)


def load_runtime_acceptance_report(
    acceptance_suite_surface: dict[str, Any],
) -> tuple[Path, dict[str, Any]]:
    runtime_acceptance_report_path = ROOT / str(acceptance_suite_surface.get("report_path", ""))
    return runtime_acceptance_report_path, load_json(runtime_acceptance_report_path)
