"""Validation timing action orchestration."""

from __future__ import annotations

import json

from ..environment import ROOT
from .validation_timing_dashboard import (
    PUBLIC_WORKFLOW_REPORT_ROOT,
    build_validation_timing_dashboard_payload,
)
from .validation_timing_markdown import write_validation_timing_markdown
from .validation_timing_budgets import validation_budget_violations
from .validation_timing_owner_contracts import require_validation_timing_owners


def action_inspect_validation_timing(_: list[str]) -> int:
    payload = build_validation_timing_dashboard_payload()
    require_validation_timing_owners(payload)
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    dashboard_path = PUBLIC_WORKFLOW_REPORT_ROOT / "validation-timing-dashboard.json"
    markdown_path = PUBLIC_WORKFLOW_REPORT_ROOT / "validation-timing-dashboard.md"
    dashboard_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    write_validation_timing_markdown(payload, markdown_path)
    print(f"summary_path: {dashboard_path.relative_to(ROOT).as_posix()}")
    print(f"dashboard_path: {markdown_path.relative_to(ROOT).as_posix()}")
    budgets = payload.get("budgets", [])
    budget_violations = (
        validation_budget_violations(budgets) if isinstance(budgets, list) else []
    )
    if budget_violations:
        print(
            "validation_timing_budget_status: FAIL "
            f"violations={len(budget_violations)}"
        )
        return 1
    return 0
