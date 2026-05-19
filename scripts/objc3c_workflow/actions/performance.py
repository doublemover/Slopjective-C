"""Performance workflow action surface."""

from __future__ import annotations

from .performance_artifacts import (
    action_build_performance_dashboard,
    action_publish_performance_report,
)
from .performance_compiler_throughput import (
    action_benchmark_compiler_throughput,
    action_validate_compiler_throughput,
)
from .performance_foundation import action_validate_performance_foundation
from .performance_governance_policy import (
    action_check_performance_governance_schema_surface,
    action_check_performance_governance_surface,
)
from .performance_governance_workflow import (
    action_validate_performance_governance,
    action_validate_performance_governance_end_to_end,
    action_validate_performance_governance_integration,
)
from .performance_metrics import (
    action_benchmark_performance,
    action_benchmark_runtime_performance,
    action_benchmark_runtime_inspector,
)
from .performance_runnable import (
    action_validate_runnable_compiler_throughput,
    action_validate_runnable_performance,
    action_validate_runnable_runtime_performance,
)
from .performance_runtime import action_validate_runtime_performance
from .performance_scenarios import action_benchmark_comparative_baselines

__all__ = [
    "action_benchmark_comparative_baselines",
    "action_benchmark_compiler_throughput",
    "action_benchmark_performance",
    "action_benchmark_runtime_inspector",
    "action_benchmark_runtime_performance",
    "action_build_performance_dashboard",
    "action_check_performance_governance_schema_surface",
    "action_check_performance_governance_surface",
    "action_publish_performance_report",
    "action_validate_compiler_throughput",
    "action_validate_performance_foundation",
    "action_validate_performance_governance",
    "action_validate_performance_governance_end_to_end",
    "action_validate_performance_governance_integration",
    "action_validate_runnable_compiler_throughput",
    "action_validate_runnable_performance",
    "action_validate_runnable_runtime_performance",
    "action_validate_runtime_performance",
]
