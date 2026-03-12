from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-C001" / "arc_lowering_abi_cleanup_model_summary.json"
CONTRACT_ID = "objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1"


def test_m262_c001_checker_emits_summary() -> None:
    subprocess.run([sys.executable, str(CHECKER), "--skip-dynamic-probes"], cwd=ROOT, check=True)
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M262-C002"
    assert payload["checks_failed"] == 0
