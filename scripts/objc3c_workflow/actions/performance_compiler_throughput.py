"""Compiler-throughput performance workflow actions."""

from __future__ import annotations

import sys

from ..commands import pwsh_file, run
from ..environment import ROOT

COMPILER_THROUGHPUT_PS1 = ROOT / "scripts" / "check_objc3c_native_perf_budget.ps1"
COMPILER_THROUGHPUT_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_compiler_throughput_integration.py"
)


def action_benchmark_compiler_throughput(rest: list[str]) -> int:
    return pwsh_file(COMPILER_THROUGHPUT_PS1, *rest)


def action_validate_compiler_throughput(_: list[str]) -> int:
    return run([sys.executable, str(COMPILER_THROUGHPUT_INTEGRATION_PY)])


__all__ = [
    "COMPILER_THROUGHPUT_INTEGRATION_PY",
    "COMPILER_THROUGHPUT_PS1",
    "action_benchmark_compiler_throughput",
    "action_validate_compiler_throughput",
]
