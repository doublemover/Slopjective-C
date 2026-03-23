from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync.py"
CONTRACT_ID = "objc3c-part6-runnable-throws-result-and-bridge-matrix/m267-e002-v1"
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-E002" / "runnable_throws_result_and_bridge_matrix_summary.json"


def test_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["issue"] == "M267-E002"
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["upstream_handoff_issue"] == "M267-E001"
    assert payload["next_issue"] == "M268-A001"
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["checks_failed"] == 0
    assert payload["failures"] == []
    chain = payload["proof_chain"]
    assert chain[0]["issue"] == "M267-A001"
    assert chain[-1]["issue"] == "M267-E001"
