"""Composite workflow step execution."""

from __future__ import annotations

from collections.abc import Sequence
from time import perf_counter

from .composite_step_nested import (
    RUNNER_SCRIPT_PATH,
    execute_nested_action,
    in_process_nested_step,
)
from .composite_step_payload import composite_step_payload, step_duration
from .composite_step_runtime_reuse import runtime_acceptance_reuse_step
from .composite_step_subprocess import subprocess_step


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


__all__ = [
    "RUNNER_SCRIPT_PATH",
    "composite_step_payload",
    "execute_nested_action",
    "in_process_nested_step",
    "run_composite_step",
    "runtime_acceptance_reuse_step",
    "step_duration",
    "subprocess_step",
]
