from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions import validation_timing
from scripts.objc3c_workflow.actions import validation_timing_reports
from scripts.objc3c_workflow.actions.validation_timing_budgets import (
    validation_budget_violations,
    validation_speed_budget_mode,
    validation_speed_budgets,
)
from scripts.objc3c_workflow.actions.validation_timing_numbers import safe_float
from scripts.objc3c_workflow.actions.validation_timing_owner_contracts import (
    VALIDATION_TIMING_REQUIRED_OWNER_KEYS,
    validation_timing_owner_payload,
)
from scripts.objc3c_workflow.actions.validation_timing_report_summaries import (
    summarize_runtime_acceptance_report,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

OWNER_MODULES = (
    "validation_timing_numbers",
    "validation_timing_report_io",
    "validation_timing_child_report_loading",
    "validation_timing_report_summaries",
    "validation_timing_dashboard_sections",
    "validation_timing_owner_contracts",
)


def test_validation_timing_reports_is_public_import_surface_only() -> None:
    facade_text = (ACTION_ROOT / "validation_timing_reports.py").read_text(
        encoding="utf-8"
    )

    for owner in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.actions.{owner}")
        assert f"from .{owner} import" in facade_text
    assert "def " not in facade_text
    assert "import json" not in facade_text
    assert "from ..environment import ROOT" not in facade_text


def test_validation_timing_public_facades_export_owner_functions() -> None:
    assert validation_timing.safe_float is safe_float
    assert validation_timing_reports.safe_float is safe_float
    assert validation_timing.summarize_runtime_acceptance_report is (
        summarize_runtime_acceptance_report
    )
    assert validation_timing_reports.summarize_runtime_acceptance_report is (
        summarize_runtime_acceptance_report
    )
    assert validation_timing.validation_timing_owner_payload is (
        validation_timing_owner_payload
    )


def test_validation_timing_summary_contract_uses_strict_command_groups() -> None:
    payload = {
        "case_count": 2,
        "default_compile_backend": "native",
        "timing": {
            "elapsed_seconds": "3.25",
            "completed_case_count": 2,
            "command_timings": [
                {"command": "objc3c-native sample.objc3", "duration_seconds": "1.5"},
                {
                    "command": "objc3c_native_compile.ps1 sample.objc3",
                    "duration_seconds": 2,
                },
            ],
        },
    }

    summary = summarize_runtime_acceptance_report(
        "tmp/report.json",
        payload,
        report_reused=True,
    )

    assert summary["elapsed_seconds"] == 3.25
    assert summary["case_count"] == 2
    assert summary["completed_case_count"] == 2
    assert summary["report_reused"] is True
    assert summary["command_groups"]["native"] == {
        "count": 1,
        "duration_seconds": 1.5,
    }
    assert summary["command_groups"]["wrapper"] == {
        "count": 1,
        "duration_seconds": 2.0,
    }


def test_validation_timing_budget_contract_is_hard_blocking() -> None:
    budgets = validation_speed_budgets(
        {"elapsed_seconds": 61.0, "command_groups": {"wrapper": {"count": 2}}},
        {"elapsed_seconds": 90.0},
        {"elapsed_seconds": 31.0},
        total_seconds=121.0,
    )

    violations = validation_budget_violations(budgets)

    assert validation_speed_budget_mode() == "fail"
    assert {budget["mode"] for budget in budgets} == {"fail"}
    assert all(
        budget["budget_owner"] == "validation_timing_budgets" for budget in budgets
    )
    assert all(
        budget["hard_blocking_decision_owner"] == "validation_timing_budgets"
        for budget in budgets
    )
    assert {violation["name"] for violation in violations} == {
        "runtime_acceptance_elapsed_seconds",
        "execution_replay_elapsed_seconds",
        "composite_elapsed_seconds",
        "runtime_acceptance_wrapper_invocations",
    }


def test_validation_timing_budget_uses_release_composite_thresholds() -> None:
    performance_governance_budgets = validation_speed_budgets(
        None,
        None,
        None,
        total_seconds=300.0,
        composite_action="validate-performance-governance",
    )
    release_foundation_budgets = validation_speed_budgets(
        None,
        None,
        None,
        total_seconds=800.0,
        composite_action="validate-release-foundation",
    )
    packaging_channels_budgets = validation_speed_budgets(
        None,
        None,
        None,
        total_seconds=900.0,
        composite_action="validate-packaging-channels",
    )
    release_operations_budgets = validation_speed_budgets(
        None,
        None,
        None,
        total_seconds=1200.0,
        composite_action="validate-release-operations",
    )
    stress_budgets = validation_speed_budgets(
        None,
        None,
        None,
        total_seconds=240.0,
        composite_action="validate-stress",
    )
    default_budgets = validation_speed_budgets(
        None,
        None,
        None,
        total_seconds=121.0,
    )

    performance_governance_composite = next(
        budget
        for budget in performance_governance_budgets
        if budget["name"] == "composite_elapsed_seconds"
    )
    release_foundation_composite = next(
        budget
        for budget in release_foundation_budgets
        if budget["name"] == "composite_elapsed_seconds"
    )
    packaging_channels_composite = next(
        budget
        for budget in packaging_channels_budgets
        if budget["name"] == "composite_elapsed_seconds"
    )
    release_operations_composite = next(
        budget
        for budget in release_operations_budgets
        if budget["name"] == "composite_elapsed_seconds"
    )
    stress_composite = next(
        budget
        for budget in stress_budgets
        if budget["name"] == "composite_elapsed_seconds"
    )
    default_composite = next(
        budget
        for budget in default_budgets
        if budget["name"] == "composite_elapsed_seconds"
    )

    assert performance_governance_composite["threshold_seconds"] == 420.0
    assert performance_governance_composite["status"] == "PASS"
    assert release_foundation_composite["threshold_seconds"] == 900.0
    assert release_foundation_composite["status"] == "PASS"
    assert packaging_channels_composite["threshold_seconds"] == 1200.0
    assert packaging_channels_composite["status"] == "PASS"
    assert release_operations_composite["threshold_seconds"] == 1500.0
    assert release_operations_composite["status"] == "PASS"
    assert stress_composite["threshold_seconds"] == 300.0
    assert stress_composite["status"] == "PASS"
    assert default_composite["threshold_seconds"] == 120.0
    assert default_composite["status"] == "FAIL"


def test_validation_timing_owner_payload_covers_blocking_contract() -> None:
    owners = validation_timing_owner_payload()

    for owner_key in VALIDATION_TIMING_REQUIRED_OWNER_KEYS:
        assert owners[owner_key]
    assert owners["budget_owner"] == "validation_timing_budgets"
    assert owners["hard_blocking_decision_owner"] == "validation_timing_budgets"
