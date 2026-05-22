"""Validation timing dashboard payload assembly."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path

from ..environment import ROOT, WORKFLOW_RUNNER_SURFACE
from .validation_timing_budgets import (
    validation_budget_violations,
    validation_speed_budgets,
)
from .validation_timing_changed_paths import (
    git_changed_paths,
    select_validation_profiles,
)
from .validation_timing_profile_rules import VALIDATION_PROFILE_RULES
from .validation_timing_dashboard_sections import dashboard_section_from_report
from .validation_timing_numbers import safe_float
from .validation_timing_owner_contracts import validation_timing_owner_payload
from .validation_timing_report_io import latest_json_file, load_latest_report_payload

PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
VALIDATION_TIMING_DASHBOARD_CONTRACT_ID = "objc3c.validation.speed.dashboard.v1"
VALIDATION_TIMING_DASHBOARD_NOTES = [
    "This dashboard is generated from the latest local timing reports under tmp.",
    "Budget failures are hard-blocking validation timing decisions.",
    "Checked-in owner modules remain the source of truth for timing decisions.",
]


def latest_validation_timing_report_paths() -> dict[str, Path | None]:
    return {
        "runtime_acceptance": latest_json_file(
            ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
        ),
        "execution_smoke": latest_json_file(
            ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke"
        ),
        "execution_replay": latest_json_file(
            ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-replay-proof"
        ),
        "test_full": latest_json_file(PUBLIC_WORKFLOW_REPORT_ROOT / "test-full.json"),
        "test_smoke": latest_json_file(PUBLIC_WORKFLOW_REPORT_ROOT / "test-smoke.json"),
    }


def build_validation_timing_dashboard_payload() -> dict[str, object]:
    report_paths = latest_validation_timing_report_paths()
    report_payloads = {
        name: load_latest_report_payload(path)
        for name, path in report_paths.items()
    }
    reports = {
        name: dashboard_section_from_report(
            name,
            report_paths[name],
            report_payloads[name],
        )
        for name in (
            "test_full",
            "test_smoke",
            "runtime_acceptance",
            "execution_smoke",
            "execution_replay",
        )
    }
    runtime_report = reports["runtime_acceptance"]
    smoke_report = reports["execution_smoke"]
    replay_report = reports["execution_replay"]
    total_seconds = safe_float(
        reports["test_full"].get("estimated_no_skip_seconds")
        or reports["test_full"].get("elapsed_seconds")
        or reports["test_smoke"].get("elapsed_seconds")
    )
    composite_action = None
    if reports["test_full"].get("status") != "MISSING":
        composite_action = "test-full"
    elif reports["test_smoke"].get("status") != "MISSING":
        composite_action = "test-smoke"
    budgets = validation_speed_budgets(
        runtime_report if runtime_report.get("status") != "MISSING" else None,
        smoke_report if smoke_report.get("status") != "MISSING" else None,
        replay_report if replay_report.get("status") != "MISSING" else None,
        total_seconds,
        composite_action=composite_action,
    )
    budget_violations = validation_budget_violations(budgets)
    return {
        "contract_id": VALIDATION_TIMING_DASHBOARD_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "owners": validation_timing_owner_payload(),
        "reports": reports,
        "budgets": budgets,
        "hard_blocking_decision": {
            "owner": "validation_timing_budgets",
            "status": "FAIL" if budget_violations else "PASS",
            "budget_violation_count": len(budget_violations),
        },
        "validation_profiles": select_validation_profiles(git_changed_paths()),
        "profile_catalog": VALIDATION_PROFILE_RULES,
        "notes": VALIDATION_TIMING_DASHBOARD_NOTES,
    }
