"""Runnable package performance workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

RUNNABLE_COMPILER_THROUGHPUT_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_compiler_throughput_end_to_end.py"
)
RUNNABLE_PERFORMANCE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_performance_end_to_end.py"
)
RUNNABLE_RUNTIME_PERFORMANCE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_runtime_performance_end_to_end.py"
)


def action_validate_runnable_compiler_throughput(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_COMPILER_THROUGHPUT_E2E_PY)])


def action_validate_runnable_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PERFORMANCE_E2E_PY)])


def action_validate_runnable_runtime_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RUNTIME_PERFORMANCE_E2E_PY)])


__all__ = [
    "RUNNABLE_COMPILER_THROUGHPUT_E2E_PY",
    "RUNNABLE_PERFORMANCE_E2E_PY",
    "RUNNABLE_RUNTIME_PERFORMANCE_E2E_PY",
    "action_validate_runnable_compiler_throughput",
    "action_validate_runnable_performance",
    "action_validate_runnable_runtime_performance",
]
