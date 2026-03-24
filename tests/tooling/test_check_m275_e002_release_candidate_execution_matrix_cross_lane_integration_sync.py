from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m275_e002_release_candidate_execution_matrix_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m275" / "M275-E002" / "pytest_summary.json"


def test_checker_static_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["contract_id"] == "objc3c-part12-release-candidate-execution-matrix/m275-e002-v1"


def test_checker_summary_records_milestone_closeout() -> None:
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["next_issue"] == "none"
