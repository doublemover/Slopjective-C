from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-E002" / "runnable_block_execution_matrix_summary.test.json"
CONTRACT_ID = "objc3c-runnable-block-execution-matrix/m261-e002-v1"


def test_m261_e002_checker_emits_summary() -> None:
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
    assert payload["next_issue"] == "M262-A001"
    assert len(payload["matrix_rows"]) == 5
    assert payload["matrix_rows"][0]["case_id"] == "owned_capture_runtime"
    assert payload["matrix_rows"][2]["expected_exit"] == 14
    assert payload["retained_d003_runtime_probe"]["dispose_count_after_final_release"] == 1
    assert payload["upstream_summaries"]["M261-E001"]["contract_id"] == "objc3c-runnable-block-runtime-gate/m261-e001-v1"
