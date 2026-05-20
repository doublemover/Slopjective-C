#!/usr/bin/env python3
"""Benchmark live objc3 workloads through the public compile and runtime paths."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file as write_json
from objc3c_tooling.subprocesses import run_capture
from objc3c_performance_benchmark.catalog import BenchmarkCatalog, load_benchmark_catalog
from objc3c_performance_benchmark.cli import parse_args as _parse_args
from objc3c_performance_benchmark.comparison import summarize_durations
from objc3c_performance_benchmark.execution import run_timed_step as _run_timed_step
from objc3c_performance_benchmark.execution import sha256_digest
from objc3c_performance_benchmark.facade import run_benchmark
from objc3c_performance_benchmark.profile import first_line, machine_profile
from objc3c_performance_benchmark.profile import tool_versions as _tool_versions
from objc3c_performance_benchmark.rendering import publish_summary, render_summary_payload
from objc3c_performance_benchmark.results import (
    benchmark_compile_workload as _benchmark_compile_workload,
    benchmark_runtime_workload as _benchmark_runtime_workload,
    expect,
)


PORTFOLIO_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_portfolio.json"
MEASUREMENT_POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "measurement_policy.json"
BENCHMARK_PARAMETERS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_parameters.json"
PERFORMANCE_BUDGET_MODEL_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "budget_model.json"
)
SUMMARY_OUT = ROOT / "tmp" / "reports" / "performance" / "benchmark-summary.json"


def parse_args(argv: Sequence[str]):
    return _parse_args(argv, summary_out=SUMMARY_OUT)


def run_timed_step(command: Sequence[str]) -> dict[str, Any]:
    return _run_timed_step(command, run_capture_fn=run_capture)


def tool_versions() -> dict[str, str]:
    return _tool_versions(run_capture_fn=run_capture)


def benchmark_compile_workload(
    workload: dict[str, Any],
    *,
    warmup_runs: int,
    measured_runs: int,
    compile_root: Path,
    profile: dict[str, Any],
    versions: dict[str, str],
    normalization_mode: str,
    measurement_policy: dict[str, Any],
    benchmark_parameters: dict[str, Any],
    budget_model: dict[str, Any],
) -> tuple[Path, list[str]]:
    return _benchmark_compile_workload(
        workload,
        warmup_runs=warmup_runs,
        measured_runs=measured_runs,
        compile_root=compile_root,
        profile=profile,
        versions=versions,
        normalization_mode=normalization_mode,
        measurement_policy=measurement_policy,
        benchmark_parameters=benchmark_parameters,
        budget_model=budget_model,
        write_json_fn=write_json,
        root=ROOT,
        run_timed_step_fn=run_timed_step,
    )


def benchmark_runtime_workload(
    workload: dict[str, Any],
    *,
    warmup_runs: int,
    measured_runs: int,
    profile: dict[str, Any],
    versions: dict[str, str],
    normalization_mode: str,
    measurement_policy: dict[str, Any],
    benchmark_parameters: dict[str, Any],
    budget_model: dict[str, Any],
) -> tuple[Path, list[str]]:
    return _benchmark_runtime_workload(
        workload,
        warmup_runs=warmup_runs,
        measured_runs=measured_runs,
        profile=profile,
        versions=versions,
        normalization_mode=normalization_mode,
        measurement_policy=measurement_policy,
        benchmark_parameters=benchmark_parameters,
        budget_model=budget_model,
        write_json_fn=write_json,
        root=ROOT,
        run_timed_step_fn=run_timed_step,
    )


def main() -> int:
    return run_benchmark(
        sys.argv[1:],
        root=ROOT,
        portfolio_path=PORTFOLIO_PATH,
        measurement_policy_path=MEASUREMENT_POLICY_PATH,
        benchmark_parameters_path=BENCHMARK_PARAMETERS_PATH,
        budget_model_path=PERFORMANCE_BUDGET_MODEL_PATH,
        summary_out=SUMMARY_OUT,
        load_json_fn=load_json,
        write_json_fn=write_json,
        run_capture_fn=run_capture,
        run_timed_step_fn=run_timed_step,
        machine_profile_fn=machine_profile,
        tool_versions_fn=tool_versions,
    )


__all__ = [
    "BENCHMARK_PARAMETERS_PATH",
    "BenchmarkCatalog",
    "MEASUREMENT_POLICY_PATH",
    "PERFORMANCE_BUDGET_MODEL_PATH",
    "PORTFOLIO_PATH",
    "ROOT",
    "SUMMARY_OUT",
    "benchmark_compile_workload",
    "benchmark_runtime_workload",
    "expect",
    "first_line",
    "load_benchmark_catalog",
    "load_json",
    "machine_profile",
    "main",
    "parse_args",
    "publish_summary",
    "render_summary_payload",
    "run_benchmark",
    "run_capture",
    "run_timed_step",
    "sha256_digest",
    "summarize_durations",
    "tool_versions",
    "write_json",
]


if __name__ == "__main__":
    raise SystemExit(main())
