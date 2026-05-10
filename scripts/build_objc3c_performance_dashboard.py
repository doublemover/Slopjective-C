#!/usr/bin/env python3
"""Build the live performance governance dashboard from upstream performance reports."""

from __future__ import annotations

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_performance_dashboard_builder import BudgetEvaluation
from objc3c_performance_dashboard_builder import DashboardInputs
from objc3c_performance_dashboard_builder import DashboardSeries
from objc3c_performance_dashboard_builder import build_dashboard_series
from objc3c_performance_dashboard_builder import build_dashboard_summary
from objc3c_performance_dashboard_builder import build_policy_contracts
from objc3c_performance_dashboard_builder import build_summary_lookup
from objc3c_performance_dashboard_builder import build_upstream_report_contracts
from objc3c_performance_dashboard_builder import collect_packet_paths
from objc3c_performance_dashboard_builder import evaluate_budget_families
from objc3c_performance_dashboard_builder import evaluate_waivers
from objc3c_performance_dashboard_builder import load_dashboard_inputs
from objc3c_performance_dashboard_builder import main
from objc3c_performance_dashboard_builder import publish_dashboard_summary
from objc3c_performance_dashboard_builder import render_dashboard_summary_json
from objc3c_performance_dashboard_builder import render_dashboard_summary_markdown
from objc3c_performance_dashboard.contracts import EXPECTED_CLAIM_STATUS_REQUIREMENTS
from objc3c_performance_dashboard.contracts import EXPECTED_POLICY_CONTRACTS
from objc3c_performance_dashboard.contracts import EXPECTED_UPSTREAM_REPORT_CONTRACTS
from objc3c_performance_dashboard.contracts import SUMMARY_CONTRACT_ID
from objc3c_performance_dashboard.input_loading import contract_id
from objc3c_performance_dashboard.input_loading import require_json
from objc3c_performance_dashboard.input_loading import validate_contracts
from objc3c_performance_dashboard.metrics import apply_waiver
from objc3c_performance_dashboard.metrics import build_breach_lookup
from objc3c_performance_dashboard.metrics import build_taxonomy_lookup
from objc3c_performance_dashboard.metrics import get_nested_value
from objc3c_performance_dashboard.metrics import load_packet_machine_profiles
from objc3c_performance_dashboard.metrics import parse_timestamp
from objc3c_performance_dashboard.metrics import report_timestamp
from objc3c_performance_dashboard.metrics import summarize_machine_profile
from objc3c_performance_dashboard.paths import BUDGET_MODEL_PATH
from objc3c_performance_dashboard.paths import CLAIM_POLICY_PATH
from objc3c_performance_dashboard.paths import COMPARATIVE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import COMPILER_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import COMPILER_SUMMARY_PATH
from objc3c_performance_dashboard.paths import LAB_POLICY_PATH
from objc3c_performance_dashboard.paths import OUTPUT_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import RUNTIME_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import RUNTIME_SUMMARY_PATH
from objc3c_performance_dashboard.paths import SOURCE_SURFACE_PATH
from objc3c_performance_dashboard.paths import TRIAGE_POLICY_PATH
from objc3c_performance_dashboard.paths import WAIVERS_PATH
from objc3c_performance_dashboard.paths import WORKFLOW_SURFACE_PATH

__all__ = [
    "BUDGET_MODEL_PATH",
    "CLAIM_POLICY_PATH",
    "COMPARATIVE_SUMMARY_PATH",
    "COMPILER_INTEGRATION_PATH",
    "COMPILER_SUMMARY_PATH",
    "EXPECTED_CLAIM_STATUS_REQUIREMENTS",
    "EXPECTED_POLICY_CONTRACTS",
    "EXPECTED_UPSTREAM_REPORT_CONTRACTS",
    "LAB_POLICY_PATH",
    "OUTPUT_PATH",
    "PERFORMANCE_INTEGRATION_PATH",
    "PERFORMANCE_SUMMARY_PATH",
    "RUNTIME_INTEGRATION_PATH",
    "RUNTIME_SUMMARY_PATH",
    "SOURCE_SURFACE_PATH",
    "SUMMARY_CONTRACT_ID",
    "TRIAGE_POLICY_PATH",
    "WAIVERS_PATH",
    "WORKFLOW_SURFACE_PATH",
    "BudgetEvaluation",
    "DashboardInputs",
    "DashboardSeries",
    "apply_waiver",
    "build_breach_lookup",
    "build_dashboard_series",
    "build_dashboard_summary",
    "build_policy_contracts",
    "build_summary_lookup",
    "build_taxonomy_lookup",
    "build_upstream_report_contracts",
    "collect_packet_paths",
    "contract_id",
    "evaluate_budget_families",
    "evaluate_waivers",
    "get_nested_value",
    "load_dashboard_inputs",
    "load_packet_machine_profiles",
    "main",
    "parse_timestamp",
    "publish_dashboard_summary",
    "render_dashboard_summary_json",
    "render_dashboard_summary_markdown",
    "report_timestamp",
    "repo_rel",
    "require_json",
    "summarize_machine_profile",
    "validate_contracts",
    "write_json_file",
]


if __name__ == "__main__":
    raise SystemExit(main())
