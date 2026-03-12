from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m262_e002_runnable_arc_conformance_matrix_execution_smoke_and_operator_docs_cross_lane_integration_sync.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-E002" / "runnable_arc_closeout_summary.test.json"
CONTRACT_ID = "objc3c-runnable-arc-closeout/m262-e002-v1"


def test_m262_e002_checker_emits_summary() -> None:
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
    assert payload["next_issue"] == "M263-A001"
    assert len(payload["matrix_rows"]) == 8
    assert payload["smoke_summary"]["status"] == "PASS"
    assert payload["upstream_summaries"]["M262-D003"]["contract_id"] == "objc3c-runtime-arc-debug-instrumentation/m262-d003-v1"
