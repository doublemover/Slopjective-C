"""Performance threshold and governance-policy workflow actions."""

from __future__ import annotations

import sys

from ..commands import pwsh_file, run
from ..environment import ROOT

COMPILER_THROUGHPUT_PS1 = ROOT / "scripts" / "check_objc3c_native_perf_budget.ps1"
COMPILER_THROUGHPUT_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_compiler_throughput_integration.py"
)
PERFORMANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_performance_integration.py"
RUNTIME_PERFORMANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_runtime_performance_integration.py"
)
PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_performance_governance_source_surface.py"
)
PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_performance_governance_schema_surface.py"
)


def action_benchmark_compiler_throughput(rest: list[str]) -> int:
    return pwsh_file(COMPILER_THROUGHPUT_PS1, *rest)


def action_validate_compiler_throughput(_: list[str]) -> int:
    return run([sys.executable, str(COMPILER_THROUGHPUT_INTEGRATION_PY)])


def action_validate_performance_foundation(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_INTEGRATION_PY)])


def action_validate_runtime_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_PERFORMANCE_INTEGRATION_PY)])


def action_check_performance_governance_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY)])


def action_check_performance_governance_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY)])
