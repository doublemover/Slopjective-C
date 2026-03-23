from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m268_e002_runnable_async_and_await_matrix_cross_lane_integration_sync.py"
CONTRACT_ID = "objc3c-part7-runnable-async-and-await-matrix/m268-e002-v1"
SUMMARY = ROOT / "tmp" / "reports" / "m268" / "M268-E002" / "runnable_async_and_await_matrix_summary.json"


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
    assert payload["issue"] == "M268-E002"
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["upstream_handoff_issue"] == "M268-E001"
    assert payload["next_issue"] == "M269-A001"
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["checks_failed"] == 0
    assert payload["failures"] == []
    rows = payload["matrix_rows"]
    assert [row["row_id"] for row in rows] == [
        "async_function_entry",
        "async_method_entry",
        "direct_call_await_lowering",
        "cleanup_integration",
        "live_continuation_helper_execution",
    ]
    assert all(row["status"] == "supported" for row in rows)
    chain = payload["proof_chain"]
    assert chain[0]["issue"] == "M268-A002"
    assert chain[-1]["issue"] == "M268-E001"
