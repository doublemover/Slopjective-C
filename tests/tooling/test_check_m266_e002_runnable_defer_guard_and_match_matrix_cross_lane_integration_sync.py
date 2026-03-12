from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m266_e002_runnable_defer_guard_and_match_matrix_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-E002" / "pytest_matrix_summary.json"
CONTRACT_ID = "objc3c-part5-runnable-control-flow-matrix/m266-e002-v1"


def test_m266_e002_checker_emits_closeout_matrix_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_issue"] == "M267-A001"
    assert len(payload["matrix_rows"]) == 4
    assert [row["row_id"] for row in payload["matrix_rows"]] == [
        "defer-ordinary-exit",
        "guard-early-return-cleanup",
        "nested-return-unwind-ordering",
        "integrated-guard-match-defer",
    ]
    assert payload["matrix_rows"][-1]["run_returncode"] == 195
