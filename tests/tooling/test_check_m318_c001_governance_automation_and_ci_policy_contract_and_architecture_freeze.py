from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_c001_governance_automation_and_ci_policy_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-C001" / "governance_automation_contract_summary.json"


def test_m318_c001_checker_passes_and_writes_summary() -> None:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-governance-automation-contract/m318-c001-v1"
    assert payload["runner_path"] == "scripts/m318_governance_guard.py"
    assert payload["workflow_path"] == ".github/workflows/m318-governance-budget-enforcement.yml"
    assert payload["stage_order"] == ["budget-snapshot", "exception-registry", "residue-proof", "topology"]
