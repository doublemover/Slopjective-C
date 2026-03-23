from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m269_e001_task_and_executor_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m269" / "M269-E001" / "task_executor_conformance_gate_summary.json"


def test_m269_e001_checker_writes_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-task-executor-conformance-gate/m269-e001-v1"
    assert payload["mode"] == "m269-e001-task-executor-conformance-gate-v1"
    assert payload["next_closeout_issue"] == "M269-E002"
    assert payload["dynamic"]["skipped"] is True
    assert payload["upstream"]["M269-D003"]["contract_id"] == "objc3c-part7-task-runtime-hardening/m269-d003-v1"
    assert payload["ok"] is True
