from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m267_e001_error_model_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-E001" / "pytest_error_model_conformance_gate_summary.json"


def test_m267_e001_checker() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["issue"] == "M267-E001"
    assert payload["contract_id"] == "objc3c-error-model-conformance-gate/m267-e001-v1"
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["dynamic_probes_executed"] is True
    assert payload["next_closeout_issue"] == "M267-E002"
    assert payload["upstream"]["M267-A002"]["contract_id"] == "objc3c-part6-error-bridge-markers/m267-a002-v1"
    assert payload["upstream"]["M267-D003"]["contract_id"] == "objc3c-cross-module-error-surface-preservation-hardening/m267-d003-v1"
    happy = payload["dynamic"]["happy_path"]
    assert happy["provider_replay"]["contract_id"] == "objc3c-part6-result-and-bridging-artifact-replay/m267-c003-v1"
    assert happy["provider_replay"]["binary_artifact_replay_ready"] is True
    assert happy["consumer_plan"]["part6_cross_module_preservation_ready"] is True
