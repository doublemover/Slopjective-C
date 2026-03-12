from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m266_b001_control_flow_and_pattern_semantic_model_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-B001" / "pytest_static_summary.json"


def test_m266_b001_static_contract_check_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["checks_failed"] == 0
    assert payload["contract_id"] == "objc3c-part5-control-flow-semantic-model/m266-b001-v1"
