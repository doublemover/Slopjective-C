"""Owner modules for Objective-C 3 performance benchmark collection."""

from __future__ import annotations

from objc3c_performance_benchmark.catalog import BenchmarkCatalog, load_benchmark_catalog
from objc3c_performance_benchmark.cli import parse_args
from objc3c_performance_benchmark.comparison import summarize_durations
from objc3c_performance_benchmark.execution import run_timed_step, sha256_digest
from objc3c_performance_benchmark.facade import main, run_benchmark
from objc3c_performance_benchmark.paths import (
    BENCHMARK_PARAMETERS_PATH,
    MEASUREMENT_POLICY_PATH,
    PORTFOLIO_PATH,
    ROOT,
    SUMMARY_OUT,
)
from objc3c_performance_benchmark.profile import first_line, machine_profile, tool_versions
from objc3c_performance_benchmark.rendering import publish_summary, render_summary_payload
from objc3c_performance_benchmark.results import (
    benchmark_compile_workload,
    benchmark_runtime_workload,
    expect,
)

__all__ = [
    "BENCHMARK_PARAMETERS_PATH",
    "BenchmarkCatalog",
    "MEASUREMENT_POLICY_PATH",
    "PORTFOLIO_PATH",
    "ROOT",
    "SUMMARY_OUT",
    "benchmark_compile_workload",
    "benchmark_runtime_workload",
    "expect",
    "first_line",
    "load_benchmark_catalog",
    "machine_profile",
    "main",
    "parse_args",
    "publish_summary",
    "render_summary_payload",
    "run_benchmark",
    "run_timed_step",
    "sha256_digest",
    "summarize_durations",
    "tool_versions",
]
