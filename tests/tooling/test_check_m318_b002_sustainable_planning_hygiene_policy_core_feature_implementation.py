from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_b002_sustainable_planning_hygiene_policy_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-B002" / "planning_hygiene_policy_summary.json"


def test_m318_b002_checker_passes_and_writes_summary() -> None:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-governance-planning-hygiene-policy/m318-b002-v1"
    assert payload["budget_impact_options"] == ["no_budget_growth", "within_existing_budget", "requires_exception_record"]
    assert ".github/ISSUE_TEMPLATE/roadmap_execution.yml" in payload["template_paths"]
    assert payload["package_governance"]["planningHygieneOwnerIssue"] == "M318-B002"
