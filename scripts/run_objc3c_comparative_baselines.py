#!/usr/bin/env python3
"""Run the checked-in ObjC2, Swift, and C++ comparative baseline workloads."""

from __future__ import annotations

try:
    from objc3c_comparative_baselines import (
        BENCHMARK_PARAMETERS_PATH,
        MANIFEST_PATH,
        MEASUREMENT_POLICY_PATH,
        ROOT,
        SUMMARY_OUT,
        BaselineRunConfig,
        BaselineRunResult,
        TimedStep,
        benchmark_baseline,
        build_arg_parser,
        build_summary_payload,
        expand_command,
        load_run_config,
        machine_profile,
        main,
        parse_args,
        record_unavailable_packet,
        render_console_result,
        run_comparative_baselines,
        run_timed_step,
        sha256_digest,
        status_from_failures,
        summarize_durations,
    )
except ModuleNotFoundError:
    from scripts.objc3c_comparative_baselines import (
        BENCHMARK_PARAMETERS_PATH,
        MANIFEST_PATH,
        MEASUREMENT_POLICY_PATH,
        ROOT,
        SUMMARY_OUT,
        BaselineRunConfig,
        BaselineRunResult,
        TimedStep,
        benchmark_baseline,
        build_arg_parser,
        build_summary_payload,
        expand_command,
        load_run_config,
        machine_profile,
        main,
        parse_args,
        record_unavailable_packet,
        render_console_result,
        run_comparative_baselines,
        run_timed_step,
        sha256_digest,
        status_from_failures,
        summarize_durations,
    )

__all__ = [
    "BENCHMARK_PARAMETERS_PATH",
    "MANIFEST_PATH",
    "MEASUREMENT_POLICY_PATH",
    "ROOT",
    "SUMMARY_OUT",
    "BaselineRunConfig",
    "BaselineRunResult",
    "TimedStep",
    "benchmark_baseline",
    "build_arg_parser",
    "build_summary_payload",
    "expand_command",
    "load_run_config",
    "machine_profile",
    "main",
    "parse_args",
    "record_unavailable_packet",
    "render_console_result",
    "run_comparative_baselines",
    "run_timed_step",
    "sha256_digest",
    "status_from_failures",
    "summarize_durations",
]

if __name__ == "__main__":
    raise SystemExit(main())
