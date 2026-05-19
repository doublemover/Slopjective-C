"""Developer-tooling and performance action specs."""

from __future__ import annotations

from .action_catalog_developer_inspection import DEVELOPER_INSPECTION_ACTION_SPECS
from .action_catalog_performance_benchmarks import PERFORMANCE_BENCHMARK_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

DEVELOPER_AND_PERFORMANCE_ACTION_SPECS: dict[str, ActionSpec] = (
    merge_action_catalog_sections(
        DEVELOPER_INSPECTION_ACTION_SPECS,
        PERFORMANCE_BENCHMARK_ACTION_SPECS,
    )
)
