"""Performance benchmark and validation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import performance

PERFORMANCE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "benchmark-runtime-inspector": performance.action_benchmark_runtime_inspector,
    "benchmark-performance": performance.action_benchmark_performance,
    "benchmark-runtime-performance": performance.action_benchmark_runtime_performance,
    "benchmark-compiler-throughput": performance.action_benchmark_compiler_throughput,
    "validate-compiler-throughput": performance.action_validate_compiler_throughput,
    "validate-runnable-compiler-throughput": performance.action_validate_runnable_compiler_throughput,
    "validate-runtime-performance": performance.action_validate_runtime_performance,
    "validate-runnable-runtime-performance": performance.action_validate_runnable_runtime_performance,
    "benchmark-comparative-baselines": performance.action_benchmark_comparative_baselines,
    "validate-runnable-performance": performance.action_validate_runnable_performance,
    "validate-performance-foundation": performance.action_validate_performance_foundation,
}
