#!/usr/bin/env python3
from __future__ import annotations

import json
import os
import subprocess
from pathlib import Path
from typing import Any
from objc3c_tooling.subprocesses import python_script_command, run_completed as run_command

try:
    from objc3c_workflow.public_command_api import public_workflow_command
except ModuleNotFoundError:
    from scripts.objc3c_workflow.public_command_api import public_workflow_command

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "closeout-gate"
SUMMARY_PATH = OUT_DIR / "governance_sustainability_closeout_gate.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
PROPOSAL_OUTPUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "new-work-proposal-closeout"
GOVERNANCE_REPORT_ROOT = ROOT / "tmp" / "reports" / "governance-sustainability"

COMMANDS = [
    {
        "name": "governance-budget-inventory",
        "command": python_script_command("scripts/build_governance_budget_inventory_summary.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "budget-inventory" / "governance_budget_inventory_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "governance-policy",
        "command": python_script_command("scripts/build_governance_policy_summary.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "sustainable-progress-policy" / "governance_policy_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "governance-maintainer-review",
        "command": python_script_command("scripts/build_governance_maintainer_review_summary.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "maintainer-review-regression" / "governance_maintainer_review_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "governance-extension-policy",
        "command": python_script_command("scripts/build_governance_extension_review_policy_summary.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "extension-review-policy" / "governance_extension_review_policy_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-extension-workflow",
        "command": python_script_command("scripts/build_governance_extension_review_workflow_summary.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "extension-review-workflow" / "governance_extension_review_workflow_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-stewardship-semantics",
        "command": python_script_command("scripts/build_governance_stewardship_semantics_summary.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "stewardship-semantics" / "governance_stewardship_semantics_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-schema-surface",
        "command": python_script_command("scripts/check_governance_sustainability_schema_surface.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "schema-surface" / "governance_schema_surface_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-artifact-contract",
        "command": python_script_command("scripts/build_governance_artifact_contract_summary.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "artifact-contract" / "governance_artifact_contract_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-budget-enforcement",
        "command": python_script_command("scripts/check_governance_sustainability_budget_enforcement.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "budget-enforcement" / "governance_budget_enforcement_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "public-command-contract",
        "command": python_script_command("scripts/build_objc3c_public_command_contract.py", "--check"),
        "summary_path": None,
    },
    {
        "name": "public-command-surface",
        "command": python_script_command("scripts/render_objc3c_public_command_surface.py", "--check"),
        "summary_path": None,
    },
    {
        "name": "public-command-budget",
        "command": python_script_command("scripts/check_objc3c_public_command_budget.py"),
        "summary_path": None,
    },
    {
        "name": "task-hygiene-gate",
        "command": python_script_command("scripts/ci/run_task_hygiene_gate.py"),
        "summary_path": None,
    },
    {
        "name": "governance-integration",
        "command": python_script_command("scripts/check_objc3c_governance_sustainability_integration.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "integration" / "governance_sustainability_integration_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-evidence",
        "command": python_script_command("scripts/build_objc3c_governance_sustainability_evidence.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "evidence-summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-publication",
        "command": python_script_command("scripts/publish_objc3c_governance_sustainability_metadata.py"),
        "summary_path": GOVERNANCE_REPORT_ROOT / "publication-summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "new-work-proposal-render",
        "command": python_script_command(
            "scripts/publish_new_work_proposal.py",
            "--proposal",
            "tests/tooling/fixtures/governance_sustainability/new_work_proposal_sample.json",
            "--output-dir",
            str(PROPOSAL_OUTPUT_DIR.relative_to(ROOT)).replace('\\', '/'),
        ),
        "summary_path": PROPOSAL_OUTPUT_DIR / "publication_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "documentation-surface",
        "command": python_script_command("scripts/check_documentation_surface.py"),
        "summary_path": None,
    },
    {
        "name": "repo-superclean-surface",
        "command": public_workflow_command("check-repo-superclean-surface"),
        "summary_path": None,
    },
]


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))




def summary_ok(payload: dict[str, Any], field: str, expected: Any | None = None) -> bool:
    if expected is None:
        return bool(payload.get(field, False))
    return payload.get(field) == expected


def main() -> int:
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    command_results: list[dict[str, Any]] = []
    ok = True
    for entry in COMMANDS:
        result = run_command(entry["command"])
        entry_ok = result.returncode == 0
        payload: dict[str, Any] = {
            "name": entry["name"],
            "command": " ".join(entry["command"]),
            "exit_code": result.returncode,
            "ok": entry_ok,
        }
        summary_path = entry.get("summary_path")
        if summary_path is not None and summary_path.is_file():
            summary_json = read_json(summary_path)
            payload["summary_path"] = str(summary_path.relative_to(ROOT)).replace("\\", "/")
            payload["summary_ok"] = summary_ok(
                summary_json,
                entry.get("summary_ok_field", "ok"),
                entry.get("summary_ok_value"),
            )
            entry_ok = entry_ok and payload["summary_ok"]
        else:
            payload["summary_path"] = None
        if not entry_ok:
            payload["stdout"] = result.stdout
            payload["stderr"] = result.stderr
        payload["ok"] = entry_ok
        command_results.append(payload)
        ok = ok and entry_ok

    summary = {
        "contract_id": "objc3c.governance.sustainability.closeout_gate.v1",
        "status": "PASS" if ok else "FAIL",
        "issue": "governance-closeout-gate",
        "runbook_mentions_closeout_gate": "check_governance_sustainability_closeout_gate.py" in runbook_text,
        "runbook_mentions_closeout_summary": str(SUMMARY_PATH.relative_to(ROOT)).replace("\\", "/") in runbook_text,
        "command_count": len(COMMANDS),
        "commands": command_results,
    }
    summary["ok"] = ok and summary["runbook_mentions_closeout_gate"] and summary["runbook_mentions_closeout_summary"]
    summary["status"] = "PASS" if summary["ok"] else "FAIL"

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
