"""Performance metric collection workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

RUNTIME_INSPECTOR_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_runtime_inspector.py"
PERFORMANCE_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_performance.py"
RUNTIME_PERFORMANCE_BENCHMARK_PY = (
    ROOT / "scripts" / "benchmark_objc3c_runtime_performance.py"
)


def action_benchmark_runtime_inspector(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_INSPECTOR_BENCHMARK_PY), *rest])


def action_benchmark_performance(rest: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_BENCHMARK_PY), *rest])


def action_benchmark_runtime_performance(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_PERFORMANCE_BENCHMARK_PY), *rest])
