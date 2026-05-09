"""Developer-tooling and performance action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import developer_tooling, performance

DEVELOPER_AND_PERFORMANCE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "inspect-capability-explorer": developer_tooling.action_inspect_capability_explorer,
    "inspect-playground-repro": developer_tooling.action_inspect_playground_repro,
    "inspect-compile-observability": developer_tooling.action_inspect_compile_observability,
    "inspect-runtime-inspector": developer_tooling.action_inspect_runtime_inspector,
    "inspect-editor-tooling": developer_tooling.action_inspect_editor_tooling,
    "format-objc3c": developer_tooling.action_format_objc3c,
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
