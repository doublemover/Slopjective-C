from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_c002_budget_enforcement_and_regression_alarms_implementation_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-C002" / "budget_enforcement_summary.json"


def test_m318_c002_checker_passes_and_writes_summary() -> None:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-governance-budget-enforcement-implementation/m318-c002-v1"
    assert payload["runner_path"] == "scripts/m318_governance_guard.py"
    assert payload["workflow_path"] == ".github/workflows/m318-governance-budget-enforcement.yml"
    assert payload["topology_ok"] is True
    assert payload["alarms"] == []
