from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class DashboardSeries:
    compile_packets: list[str]
    runtime_packets: list[str]
    derived: dict[str, float | int]
    machine_profiles_consistent: bool
    stale_report_paths: list[str]
    environment_issues: list[str]


@dataclass(frozen=True)
class BudgetEvaluation:
    budget_family_summaries: list[dict[str, Any]]
    breaches: list[dict[str, Any]]
