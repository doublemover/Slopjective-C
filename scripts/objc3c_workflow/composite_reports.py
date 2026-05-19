"""Composite workflow report writing."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path

from .actions.validation_timing import collect_child_timing
from .composite_claim_boundary import composite_claim_boundary
from .composite_report_status import (
    composite_budget_policy,
    composite_budget_violations,
    effective_composite_status,
)
from .composite_report_timing import composite_timing_payload
from .composite_surfaces import attach_child_surfaces
from .environment import ROOT, WORKFLOW_RUNNER_SURFACE

PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"


def write_composite_validation_report(
    action: str,
    steps: list[dict[str, object]],
    *,
    status: str,
) -> Path:
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    report_path = PUBLIC_WORKFLOW_REPORT_ROOT / f"{action}.json"
    child_timing = collect_child_timing(action, steps)
    budget_violations = composite_budget_violations(child_timing)
    effective_status = effective_composite_status(status, budget_violations)
    payload = {
        "action": action,
        "status": effective_status,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "timing": composite_timing_payload(steps, child_timing),
        "child_timing": child_timing,
        "budget_policy": composite_budget_policy(budget_violations),
        "claim_boundary": composite_claim_boundary(),
        "steps": steps,
    }
    attach_child_surfaces(payload, steps)
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return report_path
