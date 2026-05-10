"""Fixture execution and report loading for runtime architecture integration."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.subprocesses import python_script_command, run_capture

from .contracts import (
    FULL_HARNESS_SUMMARY_PATH,
    HARNESS_SCRIPT,
    PROOF_PACKET_PATH,
    PROOF_PACKET_SCRIPT,
    ROOT,
)


def run_runtime_architecture_inputs() -> None:
    result = run_capture(
        python_script_command(HARNESS_SCRIPT, "--run-suite", "public-test-full")
    )
    if result.returncode != 0:
        raise RuntimeError("shared executable acceptance harness failed for public-test-full")

    result = run_capture(python_script_command(PROOF_PACKET_SCRIPT))
    if result.returncode != 0:
        raise RuntimeError("runtime architecture evidence bundle validation failed")


def load_harness_summary() -> dict[str, Any]:
    return load_json(FULL_HARNESS_SUMMARY_PATH)


def load_public_workflow_report(suite_summary: dict[str, Any]) -> tuple[Path, dict[str, Any]]:
    public_workflow_report_path = ROOT / str(suite_summary.get("report_path", ""))
    return public_workflow_report_path, load_json(public_workflow_report_path)


def load_runtime_acceptance_report(
    acceptance_suite_surface: dict[str, Any],
) -> tuple[Path, dict[str, Any]]:
    runtime_acceptance_report_path = ROOT / str(acceptance_suite_surface.get("report_path", ""))
    return runtime_acceptance_report_path, load_json(runtime_acceptance_report_path)


def load_proof_packet() -> dict[str, Any]:
    return load_json(PROOF_PACKET_PATH)
