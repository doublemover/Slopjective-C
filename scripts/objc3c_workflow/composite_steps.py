"""Composite workflow step execution."""

from __future__ import annotations

import os
import sys
from collections.abc import Sequence
from pathlib import Path
from time import perf_counter

from objc3c_tooling.public_workflow_output import (
    extract_public_workflow_report_paths as extract_report_paths,
)
from objc3c_tooling.subprocesses import run_capture

RUNNER_SCRIPT_PATH = Path(__file__).with_name("runner.py").resolve()


def execute_nested_action(action: str, rest: list[str]) -> int:
    from .action_dispatch import execute_registered_action

    return execute_registered_action(action, rest)


def runtime_acceptance_reuse_step(
    action: str,
    command: Sequence[str],
    started_at: float,
) -> dict[str, object] | None:
    if (
        action.startswith("test-runtime-acceptance")
        and os.environ.get("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN") == "1"
    ):
        return {
            "action": action,
            "command": [str(token) for token in command],
            "exit_code": 0,
            "report_paths": ["tmp/reports/runtime/acceptance/summary.json"],
            "report_reused": True,
            "report_reuse_source": "OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN",
            "duration_seconds": round(perf_counter() - started_at, 6),
        }
    return None


def in_process_nested_step(
    action: str,
    normalized: list[str],
    started_at: float,
) -> dict[str, object] | None:
    if (
        len(normalized) >= 3
        and Path(normalized[0]).resolve() == Path(sys.executable).resolve()
        and Path(normalized[1]).resolve() == RUNNER_SCRIPT_PATH
    ):
        nested_action = normalized[2]
        nested_rest = normalized[3:]
        exit_code = execute_nested_action(nested_action, nested_rest)
        return {
            "action": action,
            "command": normalized,
            "exit_code": exit_code,
            "report_paths": [],
            "executed_in_process": True,
            "duration_seconds": round(perf_counter() - started_at, 6),
        }
    return None


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


def run_composite_step(action: str, command: Sequence[str]) -> dict[str, object]:
    started_at = perf_counter()
    reused_step = runtime_acceptance_reuse_step(action, command, started_at)
    if reused_step is not None:
        return reused_step
    normalized = [str(token) for token in command]
    nested_step = in_process_nested_step(action, normalized, started_at)
    if nested_step is not None:
        return nested_step
    return subprocess_step(action, command, normalized, started_at)
