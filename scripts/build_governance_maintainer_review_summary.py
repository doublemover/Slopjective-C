#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
FIXTURE_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "governance_sustainability"
    / "maintainer_review_regression_contract.json"
)
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_maintainer_workflows.md"
CONTRIBUTING_PATH = ROOT / "CONTRIBUTING.md"
OUT_DIR = ROOT / "tmp" / "reports" / "m318" / "M318-B002"
SUMMARY_PATH = OUT_DIR / "governance_maintainer_review_summary.json"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(FIXTURE_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    contributing_text = CONTRIBUTING_PATH.read_text(encoding="utf-8")

    missing_owner_surfaces = []
    missing_commands = []
    for check in contract.get("required_review_checks", []):
        owner_surface = check.get("owner_surface")
        command = check.get("command")
        if not isinstance(owner_surface, str) or not (ROOT / owner_surface).exists():
            missing_owner_surfaces.append(owner_surface)
        if not isinstance(command, str):
            missing_commands.append(str(command))

    required_mentions = {
        "runbook_mentions_governance_runbook": "docs/runbooks/objc3c_governance_sustainability.md" in runbook_text,
        "runbook_mentions_inventory_summary": "python scripts/build_governance_budget_inventory_summary.py" in runbook_text,
        "runbook_mentions_policy_summary": "python scripts/build_governance_policy_summary.py" in runbook_text,
        "runbook_mentions_review_summary": "python scripts/build_governance_maintainer_review_summary.py" in runbook_text,
        "contributing_mentions_review_summary": "python scripts/build_governance_maintainer_review_summary.py" in contributing_text,
        "contributing_mentions_policy_summary": "python scripts/build_governance_policy_summary.py" in contributing_text,
    }

    summary = {
        "issue": "M318-B002",
        "contract_id": contract["contract_id"],
        "required_review_check_count": len(contract.get("required_review_checks", [])),
        "regression_statuses": [
            entry.get("status")
            for entry in contract.get("regression_statuses", [])
            if isinstance(entry, dict)
        ],
        "missing_owner_surfaces": missing_owner_surfaces,
        "missing_commands": missing_commands,
        **required_mentions,
    }
    summary["ok"] = all([
        not missing_owner_surfaces,
        not missing_commands,
        *required_mentions.values(),
    ])

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
