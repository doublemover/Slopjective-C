from __future__ import annotations

import sys
from pathlib import Path
from typing import Any, Callable, Sequence

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file as write_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import run_capture
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from objc3c_performance_benchmark.catalog import load_benchmark_catalog
from objc3c_performance_benchmark.cli import parse_args
from objc3c_performance_benchmark.execution import run_timed_step
from objc3c_performance_benchmark.paths import (
    BENCHMARK_PARAMETERS_PATH,
    MEASUREMENT_POLICY_PATH,
    PERFORMANCE_BUDGET_MODEL_PATH,
    PORTFOLIO_PATH,
    ROOT,
    SUMMARY_OUT,
)
from objc3c_performance_benchmark.profile import machine_profile, tool_versions
from objc3c_performance_benchmark.rendering import publish_summary, render_summary_payload
from objc3c_performance_benchmark.results import benchmark_compile_workload, benchmark_runtime_workload


def run_benchmark(
    argv: Sequence[str],
    *,
    root: Path = ROOT,
    portfolio_path: Path = PORTFOLIO_PATH,
    measurement_policy_path: Path = MEASUREMENT_POLICY_PATH,
    benchmark_parameters_path: Path = BENCHMARK_PARAMETERS_PATH,
    budget_model_path: Path = PERFORMANCE_BUDGET_MODEL_PATH,
    summary_out: Path = SUMMARY_OUT,
    load_json_fn: Callable[[Path], dict[str, Any]] = load_json,
    write_json_fn: Callable[[Path, dict[str, Any]], None] = write_json,
    run_capture_fn: Callable[[Sequence[str]], Any] = run_capture,
    run_timed_step_fn: Callable[[Sequence[str]], dict[str, Any]] = run_timed_step,
    machine_profile_fn: Callable[[], dict[str, Any]] = machine_profile,
    tool_versions_fn: Callable[[], dict[str, str]] = tool_versions,
) -> int:
    args = parse_args(argv, summary_out=summary_out)
    catalog = load_benchmark_catalog(
        portfolio_path=portfolio_path,
        measurement_policy_path=measurement_policy_path,
        benchmark_parameters_path=benchmark_parameters_path,
        load_json_fn=load_json_fn,
        warmup_runs_override=args.warmup_runs,
        measured_runs_override=args.measured_runs,
    )
    measurement_policy = load_json_fn(measurement_policy_path)
    benchmark_parameters = load_json_fn(benchmark_parameters_path)
    budget_model = load_json_fn(budget_model_path)
    compile_root = root / "tmp" / "artifacts" / "performance" / "compile"
    compile_root.mkdir(parents=True, exist_ok=True)

    build_result = run_capture_fn(public_workflow_command("build-native-binaries"))
    if build_result.returncode != 0:
        raise RuntimeError("build-native-binaries failed before benchmarking")

    profile = machine_profile_fn()
    versions = tool_versions_fn()
    packet_paths: list[str] = []
    failures: list[str] = []

    for workload in catalog.workloads:
        compile_packet_path, compile_failures = benchmark_compile_workload(
            workload,
            warmup_runs=catalog.warmup_runs,
            measured_runs=catalog.measured_runs,
            compile_root=compile_root,
            profile=profile,
            versions=versions,
            normalization_mode=catalog.normalization_mode,
            measurement_policy=measurement_policy,
            benchmark_parameters=benchmark_parameters,
            budget_model=budget_model,
            root=root,
            write_json_fn=write_json_fn,
            run_timed_step_fn=run_timed_step_fn,
        )
        packet_paths.append(repo_rel(compile_packet_path, root=root))
        failures.extend(compile_failures)

        runtime_packet_path, runtime_failures = benchmark_runtime_workload(
            workload,
            warmup_runs=catalog.warmup_runs,
            measured_runs=catalog.measured_runs,
            profile=profile,
            versions=versions,
            normalization_mode=catalog.normalization_mode,
            measurement_policy=measurement_policy,
            benchmark_parameters=benchmark_parameters,
            budget_model=budget_model,
            root=root,
            write_json_fn=write_json_fn,
            run_timed_step_fn=run_timed_step_fn,
        )
        packet_paths.append(repo_rel(runtime_packet_path, root=root))
        failures.extend(runtime_failures)

    payload = render_summary_payload(
        failures=failures,
        packet_paths=packet_paths,
        portfolio_path=portfolio_path,
        measurement_policy_path=measurement_policy_path,
        benchmark_parameters_path=benchmark_parameters_path,
        root=root,
    )
    return publish_summary(args.summary_out, payload, failures=failures, write_json_fn=write_json_fn)


def main(argv: Sequence[str] | None = None) -> int:
    return run_benchmark(sys.argv[1:] if argv is None else argv)
