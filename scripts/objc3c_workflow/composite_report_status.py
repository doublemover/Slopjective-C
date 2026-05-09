"""Composite workflow report status and budget policy fields."""

from __future__ import annotations

from .actions.validation_timing import (
    validation_budget_violations,
    validation_speed_budget_mode,
)
from .composite_report_policy import COMPOSITE_STATUS_FAIL, COMPOSITE_STATUS_PASS


def composite_budget_violations(child_timing: dict[str, object]) -> list[object]:
    budgets = child_timing.get("budgets", [])
    return validation_budget_violations(budgets) if isinstance(budgets, list) else []


def effective_composite_status(status: str, budget_violations: list[object]) -> str:
    return COMPOSITE_STATUS_FAIL if status == COMPOSITE_STATUS_PASS and budget_violations else status


def composite_budget_policy(budget_violations: list[object]) -> dict[str, object]:
    return {
        "contract_id": "objc3c.validation.speed.budget.policy.v1",
        "mode": (
            "fail"
            if validation_speed_budget_mode() == "fail"
            else "warning-only"
        ),
        "violations": budget_violations,
        "fail_closed": True,
    }


__all__ = [
    "composite_budget_policy",
    "composite_budget_violations",
    "effective_composite_status",
]
