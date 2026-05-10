"""Implementation modules for the objc3c comparative baseline runner."""

from __future__ import annotations

from .catalog import BaselineRunConfig, load_run_config
from .cli import build_arg_parser, main, parse_args
from .comparison import status_from_failures
from .execution import TimedStep, expand_command, run_timed_step
from .paths import BENCHMARK_PARAMETERS_PATH, MANIFEST_PATH, MEASUREMENT_POLICY_PATH, ROOT, SUMMARY_OUT
from .profiles import machine_profile
from .reporting import build_summary_payload, render_console_result
from .runner import BaselineRunResult, benchmark_baseline, run_comparative_baselines
from .telemetry import record_unavailable_packet, sha256_digest, summarize_durations

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
