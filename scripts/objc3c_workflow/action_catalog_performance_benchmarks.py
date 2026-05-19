"""Performance benchmark and validation action specs."""

from __future__ import annotations

from .action_catalog_performance_core_benchmarks import (
    PERFORMANCE_CORE_BENCHMARK_ACTION_SPECS,
)
from .action_catalog_performance_packaging import PERFORMANCE_PACKAGING_ACTION_SPECS
from .action_catalog_performance_runtime import PERFORMANCE_RUNTIME_ACTION_SPECS
from .action_catalog_performance_throughput import PERFORMANCE_THROUGHPUT_ACTION_SPECS
from .action_spec import ActionSpec

PERFORMANCE_BENCHMARK_ACTION_SPECS: dict[str, ActionSpec] = {
    **PERFORMANCE_CORE_BENCHMARK_ACTION_SPECS,
    **PERFORMANCE_RUNTIME_ACTION_SPECS,
    **PERFORMANCE_THROUGHPUT_ACTION_SPECS,
    **PERFORMANCE_PACKAGING_ACTION_SPECS,
}

__all__ = ["PERFORMANCE_BENCHMARK_ACTION_SPECS"]
