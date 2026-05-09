"""Composite workflow report writing."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path

from .actions.validation_timing import (
    collect_child_timing,
    validation_budget_violations,
    validation_speed_budget_mode,
)
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
    child_timing = collect_child_timing(steps)
    budgets = child_timing.get("budgets", [])
    budget_violations = (
        validation_budget_violations(budgets) if isinstance(budgets, list) else []
    )
    effective_status = "FAIL" if status == "PASS" and budget_violations else status
    payload = {
        "action": action,
        "status": effective_status,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "timing": {
            "step_count": len(steps),
            "total_step_duration_seconds": round(
                sum(float(step.get("duration_seconds", 0.0)) for step in steps),
                6,
            ),
            "slowest_steps": sorted(
                [
                    {
                        "action": str(step.get("action", "")),
                        "duration_seconds": float(step.get("duration_seconds", 0.0)),
                        "exit_code": int(step.get("exit_code", 0)),
                    }
                    for step in steps
                ],
                key=lambda entry: entry["duration_seconds"],
                reverse=True,
            )[:10],
            "estimated_no_skip_seconds": child_timing["estimated_no_skip_seconds"],
        },
        "child_timing": child_timing,
        "budget_policy": {
            "contract_id": "objc3c.validation.speed.budget.policy.v1",
            "mode": (
                "fail"
                if validation_speed_budget_mode() == "fail"
                else "warning-only"
            ),
            "violations": budget_violations,
            "fail_closed": True,
        },
        "claim_boundary": {
            "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
            "reports_are_authoritative_only_when_child_steps_are_compile-coupled": True,
            "authoritative_child_surfaces": [
                "scripts/check_objc3c_runtime_acceptance.py",
                "scripts/check_objc3c_execution_replay_proof.ps1",
                "scripts/check_objc3c_native_execution_smoke.ps1",
            ],
            "non_authoritative_inputs": [
                "integrated report paths by themselves",
                "sidecar-only summaries with no matching emitted object/probe path",
                "synthetic or hand-authored llvm ir used without coupled compile output",
            ],
        },
        "steps": steps,
    }
    attach_child_surfaces(payload, steps)
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return report_path
