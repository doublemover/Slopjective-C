from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m260_e001_ownership_runtime_gate_contract_and_architecture_freeze.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m260" / "M260-E001" / "ownership_runtime_gate_contract_summary.test.json"
CONTRACT_ID = "objc3c-ownership-runtime-gate-freeze/m260-e001-v1"


def test_m260_e001_checker_emits_summary() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "--skip-dynamic-probes",
            "--summary-out",
            str(TEST_SUMMARY),
        ],
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
    assert payload["dynamic_probes_executed"] is False
    assert payload["dynamic_case"]["skipped"] is True
    assert payload["next_closeout_issue"] == "M260-E002"
    assert payload["upstream_summaries"]["M260-C002"]["contract_id"] == "objc3c-ownership-runtime-hook-emission/m260-c002-v1"
    assert payload["upstream_summaries"]["M260-D001"]["contract_id"] == "objc3c-runtime-memory-management-api-freeze/m260-d001-v1"
    assert payload["upstream_summaries"]["M260-D002"]["contract_id"] == "objc3c-runtime-memory-management-implementation/m260-d002-v1"
