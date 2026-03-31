#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "m318" / "M318-E001"
SUMMARY_PATH = OUT_DIR / "governance_hardening_closeout_gate.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
PROPOSAL_OUTPUT_DIR = ROOT / "tmp" / "reports" / "m318" / "M318-D001" / "new_work_proposal_closeout"

COMMANDS = [
    {
        "name": "governance-budget-inventory",
        "command": ["python", "scripts/build_governance_budget_inventory_summary.py"],
        "summary_path": ROOT / "tmp/reports/m318/M318-A001/governance_budget_inventory_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "governance-policy",
        "command": ["python", "scripts/build_governance_policy_summary.py"],
        "summary_path": ROOT / "tmp/reports/m318/M318-B001/governance_policy_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "governance-maintainer-review",
        "command": ["python", "scripts/build_governance_maintainer_review_summary.py"],
        "summary_path": ROOT / "tmp/reports/m318/M318-B002/governance_maintainer_review_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "governance-schema-surface",
        "command": ["python", "scripts/check_governance_sustainability_schema_surface.py"],
        "summary_path": ROOT / "tmp/reports/m318/M318-C001/governance_schema_surface_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-budget-enforcement",
        "command": ["python", "scripts/check_governance_sustainability_budget_enforcement.py"],
        "summary_path": ROOT / "tmp/reports/m318/M318-C002/governance_budget_enforcement_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "governance-anti-regression",
        "command": ["python", "scripts/build_governance_anti_regression_summary.py"],
        "summary_path": ROOT / "tmp/reports/m318/M318-D002/governance_anti_regression_summary.json",
        "summary_ok_field": "status",
        "summary_ok_value": "PASS",
    },
    {
        "name": "new-work-proposal-render",
        "command": [
            "python",
            "tmp/github-publish/m318_governance/publish_new_work_proposal.py",
            "--proposal",
            "tmp/planning/m318_governance/new_work_proposal_sample.json",
            "--output-dir",
            str(PROPOSAL_OUTPUT_DIR.relative_to(ROOT)).replace('\\', '/'),
        ],
        "summary_path": PROPOSAL_OUTPUT_DIR / "publication_summary.json",
        "summary_ok_field": "ok",
    },
    {
        "name": "task-hygiene-gate",
        "command": ["python", "scripts/ci/run_task_hygiene_gate.py"],
        "summary_path": None,
    },
    {
        "name": "documentation-surface",
        "command": ["python", "scripts/check_documentation_surface.py"],
        "summary_path": None,
    },
    {
        "name": "repo-superclean-surface",
        "command": ["python", "scripts/check_repo_superclean_surface.py"],
        "summary_path": None,
    },
]


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)


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
        "issue": "M318-E001",
        "runbook_mentions_closeout_gate": "check_governance_sustainability_closeout_gate.py" in runbook_text,
        "command_count": len(COMMANDS),
        "commands": command_results,
    }
    summary["ok"] = ok and summary["runbook_mentions_closeout_gate"]

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
