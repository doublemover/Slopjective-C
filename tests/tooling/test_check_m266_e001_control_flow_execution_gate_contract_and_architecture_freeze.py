from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m266_e001_control_flow_execution_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-E001" / "pytest_gate_summary.json"
CONTRACT_ID = "objc3c-part5-control-flow-execution-gate/m266-e001-v1"


def test_m266_e001_checker_emits_integrated_gate_summary() -> None:
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
    assert payload["dynamic_probes_executed"] is True
    assert payload["next_closeout_issue"] == "M266-E002"
    assert payload["integrated_probe"]["compile_returncode"] == 0
    assert payload["integrated_probe"]["run_returncode"] == 195
    replay_map = payload["integrated_probe"]["manifest_lowering_replay_key_map"]
    assert replay_map["guard_statement_sites"] == "1"
    assert replay_map["match_statement_sites"] == "1"
    assert replay_map["defer_statement_sites"] == "1"
    assert replay_map["live_guard_short_circuit_sites"] == "1"
    assert replay_map["live_match_dispatch_sites"] == "1"
    assert replay_map["live_defer_cleanup_sites"] == "1"
