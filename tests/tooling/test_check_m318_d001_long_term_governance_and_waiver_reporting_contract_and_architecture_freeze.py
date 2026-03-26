from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_d001_long_term_governance_and_waiver_reporting_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-D001" / "long_term_governance_reporting_summary.json"


def test_d001_checker_produces_green_summary() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c-governance-long-term-reporting-contract/m318-d001-v1"
