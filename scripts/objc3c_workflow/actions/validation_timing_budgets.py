"""Validation timing budget policy helpers."""

from __future__ import annotations

from collections.abc import Sequence

from .validation_timing_numbers import safe_float


VALIDATION_TIMING_BUDGET_OWNER = "validation_timing_budgets"
VALIDATION_TIMING_HARD_BLOCKING_DECISION_OWNER = "validation_timing_budgets"
DEFAULT_COMPOSITE_ELAPSED_THRESHOLD_SECONDS = 120.0
COMPOSITE_ELAPSED_THRESHOLD_SECONDS_BY_ACTION = {
    "validate-performance-governance": 420.0,
    "validate-release-foundation": 900.0,
    "validate-packaging-channels": 1200.0,
    "validate-release-operations": 1500.0,
    "validate-distribution-credibility": 1500.0,
    "validate-security-hardening": 1800.0,
    "validate-stress": 300.0,
}


def validation_speed_budget_mode() -> str:
    return "fail"


def composite_elapsed_threshold_seconds(composite_action: str | None) -> float:
    if composite_action is None:
        return DEFAULT_COMPOSITE_ELAPSED_THRESHOLD_SECONDS
    return COMPOSITE_ELAPSED_THRESHOLD_SECONDS_BY_ACTION.get(
        composite_action,
        DEFAULT_COMPOSITE_ELAPSED_THRESHOLD_SECONDS,
    )


def validation_speed_budgets(
    runtime_acceptance: dict[str, object] | None,
    execution_smoke: dict[str, object] | None,
    execution_replay: dict[str, object] | None,
    total_seconds: float,
    *,
    composite_action: str | None = None,
) -> list[dict[str, object]]:
    budget_mode = validation_speed_budget_mode()
    budgets = [
        {
            "name": "runtime_acceptance_elapsed_seconds",
            "threshold_seconds": 60.0,
            "actual_seconds": safe_float(
                runtime_acceptance.get("elapsed_seconds") if runtime_acceptance else None
            ),
        },
        {
            "name": "execution_smoke_elapsed_seconds",
            "threshold_seconds": 90.0,
            "actual_seconds": safe_float(
                execution_smoke.get("elapsed_seconds") if execution_smoke else None
            ),
        },
        {
            "name": "execution_replay_elapsed_seconds",
            "threshold_seconds": 30.0,
            "actual_seconds": safe_float(
                execution_replay.get("elapsed_seconds") if execution_replay else None
            ),
        },
        {
            "name": "composite_elapsed_seconds",
            "threshold_seconds": composite_elapsed_threshold_seconds(
                composite_action
            ),
            "actual_seconds": total_seconds,
        },
    ]
    for budget in budgets:
        actual = safe_float(budget["actual_seconds"])
        threshold = safe_float(budget["threshold_seconds"])
        if actual == 0.0:
            budget["status"] = "UNKNOWN"
        elif actual <= threshold:
            budget["status"] = "PASS"
        else:
            budget["status"] = "FAIL"
        budget["mode"] = budget_mode
        budget["budget_owner"] = VALIDATION_TIMING_BUDGET_OWNER
        budget["hard_blocking_decision_owner"] = (
            VALIDATION_TIMING_HARD_BLOCKING_DECISION_OWNER
        )
    if runtime_acceptance is not None:
        command_groups = runtime_acceptance.get("command_groups", {})
        wrapper_count = None
        if isinstance(command_groups, dict):
            wrapper = command_groups.get("wrapper", {})
            if isinstance(wrapper, dict):
                wrapper_count = wrapper.get("count")
        budgets.append(
            {
                "name": "runtime_acceptance_wrapper_invocations",
                "threshold_count": 1,
                "actual_count": wrapper_count,
                "status": "PASS"
                if isinstance(wrapper_count, int) and wrapper_count <= 1
                else "UNKNOWN"
                if wrapper_count is None
                else "FAIL",
                "mode": budget_mode,
                "budget_owner": VALIDATION_TIMING_BUDGET_OWNER,
                "hard_blocking_decision_owner": (
                    VALIDATION_TIMING_HARD_BLOCKING_DECISION_OWNER
                ),
            }
        )
    return budgets


def validation_budget_violations(
    budgets: Sequence[dict[str, object]],
) -> list[dict[str, object]]:
    return [
        budget
        for budget in budgets
        if budget.get("mode") == "fail" and budget.get("status") == "FAIL"
    ]
