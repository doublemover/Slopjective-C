from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m267_c001_throws_abi_and_propagation_lowering_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-C001" / "throws_abi_and_propagation_lowering_summary.json"


def test_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(SCRIPT), "--summary-out", str(SUMMARY)], cwd=ROOT, check=False)
    assert completed.returncode == 0
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m267-c001-throws-abi-and-propagation-lowering-contract-and-architecture-freeze-v1"
    assert payload["contract_id"] == "objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1"
    assert payload["checks_failed"] == 0
    assert payload["dynamic_probes_executed"] is True
