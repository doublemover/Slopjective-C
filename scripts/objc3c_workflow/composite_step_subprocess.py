"""Subprocess execution for composite workflow steps."""

from __future__ import annotations

from collections.abc import Sequence
from time import perf_counter

from objc3c_tooling.public_workflow_output import (
    extract_public_workflow_report_paths as extract_report_paths,
)
from objc3c_tooling.subprocesses import run_capture


def subprocess_step(
    action: str,
    command: Sequence[str],
    normalized: list[str],
    started_at: float,
) -> dict[str, object]:
    result = run_capture(command)
    return {
        "action": action,
        "command": normalized,
        "exit_code": result.returncode,
        "report_paths": extract_report_paths(result.stdout),
        "duration_seconds": round(perf_counter() - started_at, 6),
    }
