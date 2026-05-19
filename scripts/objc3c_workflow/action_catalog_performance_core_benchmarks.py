"""Core benchmark action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PERFORMANCE_CORE_BENCHMARK_ACTION_SPECS: dict[str, ActionSpec] = {
    "benchmark-runtime-inspector": ActionSpec(
        "benchmark-runtime-inspector",
        "measure the live runtime-inspector and capability-explorer workflow and write a reproducible benchmark report",
        "python:scripts/benchmark_objc3c_runtime_inspector.py",
        validation_tier="repo",
        guarantee_owner=(
            "runtime inspector timing and capability comparisons stay tied to executable "
            "public actions and real emitted artifacts"
        ),
        pass_through_args=True,
    ),
    "benchmark-performance": ActionSpec(
        "benchmark-performance",
        "measure the checked-in objc3 showcase workloads and write reproducible compile/runtime telemetry packets",
        "python:scripts/benchmark_objc3c_performance.py",
        validation_tier="repo",
        guarantee_owner=(
            "objc3 benchmark telemetry stays tied to checked-in showcase workloads "
            "and raw sample packets"
        ),
        pass_through_args=True,
    ),
}
