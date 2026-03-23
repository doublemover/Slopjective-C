from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m270_e002_runnable_actor_and_isolation_matrix_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-E002" / "runnable_actor_and_isolation_matrix_summary.json"


def test_m270_e002_checker_writes_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-part7-runnable-actor-isolation-matrix/m270-e002-v1"
    assert payload["mode"] == "m270-e002-runnable-actor-and-isolation-matrix-cross-lane-integration-sync-v1"
    assert payload["next_issue"] == "M271-A001"
    assert payload["dynamic_probes_executed"] is False
    assert payload["upstream_handoff_issue"] == "M270-E001"
    assert payload["ok"] is True
