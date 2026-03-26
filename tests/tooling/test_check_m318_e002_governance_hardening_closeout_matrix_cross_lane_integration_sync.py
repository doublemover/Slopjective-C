from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_e002_governance_hardening_closeout_matrix_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-E002" / "governance_hardening_closeout_matrix_summary.json"


def test_e002_checker_produces_green_summary() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c-governance-hardening-closeout-matrix/m318-e002-v1"
