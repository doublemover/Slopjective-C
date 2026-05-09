"""Performance, benchmark, and performance-governance workflow actions."""

from __future__ import annotations

import sys

from ..composite_validation import run_composite_validation
from ..commands import pwsh_file, run
from ..environment import ROOT

COMPILER_THROUGHPUT_PS1 = ROOT / "scripts" / "check_objc3c_native_perf_budget.ps1"
RUNTIME_INSPECTOR_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_runtime_inspector.py"
PERFORMANCE_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_performance.py"
RUNTIME_PERFORMANCE_BENCHMARK_PY = (
    ROOT / "scripts" / "benchmark_objc3c_runtime_performance.py"
)
COMPILER_THROUGHPUT_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_compiler_throughput_integration.py"
)
RUNNABLE_COMPILER_THROUGHPUT_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_compiler_throughput_end_to_end.py"
)
COMPARATIVE_BASELINES_PY = ROOT / "scripts" / "run_objc3c_comparative_baselines.py"
RUNNABLE_PERFORMANCE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_performance_end_to_end.py"
)
PERFORMANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_performance_integration.py"
RUNTIME_PERFORMANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_runtime_performance_integration.py"
)
RUNNABLE_RUNTIME_PERFORMANCE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_runtime_performance_end_to_end.py"
)
PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_performance_governance_source_surface.py"
)
PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_performance_governance_schema_surface.py"
)
PERFORMANCE_GOVERNANCE_DASHBOARD_PY = (
    ROOT / "scripts" / "build_objc3c_performance_dashboard.py"
)
PERFORMANCE_GOVERNANCE_REPORT_PY = (
    ROOT / "scripts" / "publish_objc3c_performance_report.py"
)
PERFORMANCE_GOVERNANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_performance_governance_integration.py"
)
PERFORMANCE_GOVERNANCE_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_performance_governance_end_to_end.py"
)


def action_benchmark_runtime_inspector(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_INSPECTOR_BENCHMARK_PY), *rest])


def action_benchmark_performance(rest: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_BENCHMARK_PY), *rest])


def action_benchmark_runtime_performance(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_PERFORMANCE_BENCHMARK_PY), *rest])


def action_benchmark_compiler_throughput(rest: list[str]) -> int:
    return pwsh_file(COMPILER_THROUGHPUT_PS1, *rest)


def action_benchmark_comparative_baselines(rest: list[str]) -> int:
    return run([sys.executable, str(COMPARATIVE_BASELINES_PY), *rest])


def action_validate_compiler_throughput(_: list[str]) -> int:
    return run([sys.executable, str(COMPILER_THROUGHPUT_INTEGRATION_PY)])


def action_validate_runnable_compiler_throughput(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_COMPILER_THROUGHPUT_E2E_PY)])


def action_validate_runtime_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_PERFORMANCE_INTEGRATION_PY)])


def action_validate_runnable_runtime_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RUNTIME_PERFORMANCE_E2E_PY)])


def action_validate_runnable_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PERFORMANCE_E2E_PY)])


def action_validate_performance_foundation(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_INTEGRATION_PY)])


def action_check_performance_governance_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY)])


def action_check_performance_governance_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY)])


def action_build_performance_dashboard(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_DASHBOARD_PY)])


def action_publish_performance_report(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_REPORT_PY)])


def action_validate_performance_governance(_: list[str]) -> int:
    return run_composite_validation(
        "validate-performance-governance",
        [
            (
                "validate-performance-foundation",
                [sys.executable, str(PERFORMANCE_INTEGRATION_PY)],
            ),
            (
                "validate-compiler-throughput",
                [sys.executable, str(COMPILER_THROUGHPUT_INTEGRATION_PY)],
            ),
            (
                "validate-runtime-performance",
                [sys.executable, str(RUNTIME_PERFORMANCE_INTEGRATION_PY)],
            ),
            (
                "check-performance-governance-surface",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY)],
            ),
            (
                "check-performance-governance-schema-surface",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY)],
            ),
            (
                "build-performance-dashboard",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_DASHBOARD_PY)],
            ),
            (
                "publish-performance-report",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_REPORT_PY)],
            ),
        ],
    )


def action_validate_performance_governance_integration(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_INTEGRATION_PY)])


def action_validate_performance_governance_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_END_TO_END_PY)])
