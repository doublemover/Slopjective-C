"""In-process nested action execution for composite workflow steps."""

from __future__ import annotations

import sys
from pathlib import Path
from time import perf_counter

RUNNER_SCRIPT_PATH = Path(__file__).with_name("runner.py").resolve()


def execute_nested_action(action: str, rest: list[str]) -> int:
    from .action_dispatch import execute_registered_action

    return execute_registered_action(action, rest)


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
