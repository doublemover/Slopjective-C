from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m260_e002_runnable_ownership_smoke_matrix_and_docs_cross_lane_integration_sync.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-E002" / "runnable_ownership_smoke_matrix_summary.test.json"
CONTRACT_ID = "objc3c-runnable-ownership-smoke-matrix/m260-e002-v1"


def test_m260_e002_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(TEST_SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_issue"] == "M261-A001"
    assert len(payload["matrix_rows"]) == 5
    assert payload["upstream_summaries"]["M260-D002"]["contract_id"] == "objc3c-runtime-memory-management-implementation/m260-d002-v1"
    assert payload["upstream_summaries"]["M260-E001"]["contract_id"] == "objc3c-ownership-runtime-gate-freeze/m260-e001-v1"
