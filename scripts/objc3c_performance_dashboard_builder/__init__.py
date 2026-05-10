"""Builder package for the Objective-C 3 performance governance dashboard."""

from __future__ import annotations

from objc3c_performance_dashboard_builder.cli import main
from objc3c_performance_dashboard_builder.input_loading import (
    DashboardInputs,
    build_policy_contracts,
    build_upstream_report_contracts,
    load_dashboard_inputs,
)
from objc3c_performance_dashboard_builder.rendering import (
    publish_dashboard_summary,
    render_dashboard_summary_json,
    render_dashboard_summary_markdown,
)
from objc3c_performance_dashboard_builder.series import (
    BudgetEvaluation,
    DashboardSeries,
    build_dashboard_series,
    build_summary_lookup,
    collect_packet_paths,
    evaluate_budget_families,
    evaluate_waivers,
)
from objc3c_performance_dashboard_builder.summary import build_dashboard_summary

__all__ = [
    "BudgetEvaluation",
    "DashboardInputs",
    "DashboardSeries",
    "build_dashboard_series",
    "build_dashboard_summary",
    "build_policy_contracts",
    "build_summary_lookup",
    "build_upstream_report_contracts",
    "collect_packet_paths",
    "evaluate_budget_families",
    "evaluate_waivers",
    "load_dashboard_inputs",
    "main",
    "publish_dashboard_summary",
    "render_dashboard_summary_json",
    "render_dashboard_summary_markdown",
]
