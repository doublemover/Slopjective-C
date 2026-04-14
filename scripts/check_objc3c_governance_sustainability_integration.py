#!/usr/bin/env python3
"""Validate the integrated governance and extension-review workflow."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_timed


ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "governance-sustainability" / "integration" / "governance_sustainability_integration_summary.json"

STEPS = [
    (
        "budget-inventory",
        [sys.executable, "scripts/build_governance_budget_inventory_summary.py"],
        "tmp/reports/governance-sustainability/budget-inventory/governance_budget_inventory_summary.json",
    ),
    (
        "sustainable-progress-policy",
        [sys.executable, "scripts/build_governance_policy_summary.py"],
        "tmp/reports/governance-sustainability/sustainable-progress-policy/governance_policy_summary.json",
    ),
    (
        "maintainer-review",
        [sys.executable, "scripts/build_governance_maintainer_review_summary.py"],
        "tmp/reports/governance-sustainability/maintainer-review-regression/governance_maintainer_review_summary.json",
    ),
    (
        "extension-review-policy",
        [sys.executable, "scripts/build_governance_extension_review_policy_summary.py"],
        "tmp/reports/governance-sustainability/extension-review-policy/governance_extension_review_policy_summary.json",
    ),
    (
        "extension-review-workflow",
        [sys.executable, "scripts/build_governance_extension_review_workflow_summary.py"],
        "tmp/reports/governance-sustainability/extension-review-workflow/governance_extension_review_workflow_summary.json",
    ),
    (
        "stewardship-semantics",
        [sys.executable, "scripts/build_governance_stewardship_semantics_summary.py"],
        "tmp/reports/governance-sustainability/stewardship-semantics/governance_stewardship_semantics_summary.json",
    ),
    (
        "schema-surface",
        [sys.executable, "scripts/check_governance_sustainability_schema_surface.py"],
        "tmp/reports/governance-sustainability/schema-surface/governance_schema_surface_summary.json",
    ),
    (
        "artifact-contract",
        [sys.executable, "scripts/build_governance_artifact_contract_summary.py"],
        "tmp/reports/governance-sustainability/artifact-contract/governance_artifact_contract_summary.json",
    ),
    (
        "budget-enforcement",
        [sys.executable, "scripts/check_governance_sustainability_budget_enforcement.py"],
        "tmp/reports/governance-sustainability/budget-enforcement/governance_budget_enforcement_summary.json",
    ),
    (
        "anti-regression",
        [sys.executable, "scripts/build_governance_anti_regression_summary.py"],
        "tmp/reports/governance-sustainability/anti-regression/governance_anti_regression_summary.json",
    ),
]




def run_step(label: str, command: list[str], summary_path: str) -> dict[str, Any]:
    completed = run_timed(command, cwd=ROOT, echo=True)
    summary_file = ROOT / summary_path
    summary = load_json(summary_file) if summary_file.is_file() else {}
    status = summary.get("status")
    ok = completed.returncode == 0 and bool(summary.get("ok", status == "PASS"))
    return {
        "label": label,
        "command": command,
        "summary_path": summary_path,
        "exit_code": completed.returncode,
        "duration_ms": completed.duration_ms,
        "status": status,
        "ok": ok,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def main() -> int:
    step_results = [run_step(label, command, summary_path) for label, command, summary_path in STEPS]
    failures = [
        f"{result['label']} failed with exit {result['exit_code']}"
        for result in step_results
        if not result["ok"]
    ]

    payload = {
        "contract_id": "objc3c.governance.sustainability.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_governance_sustainability_integration.py",
        "step_count": len(step_results),
        "steps": step_results,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-governance-sustainability-integration: PASS" if not failures else "objc3c-governance-sustainability-integration: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
