from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_e001_sustainable_progress_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-E001" / "sustainable_progress_gate_summary.json"


def test_e001_checker_produces_green_summary() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c-governance-sustainable-progress-gate/m318-e001-v1"
