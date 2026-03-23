from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m268_e001_async_executable_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m268" / "M268-E001" / "pytest_async_executable_conformance_gate_summary.json"


def test_m268_e001_checker() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["issue"] == "M268-E001"
    assert payload["contract_id"] == "objc3c-async-executable-conformance-gate/m268-e001-v1"
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["dynamic_probes_executed"] is True
    assert payload["next_closeout_issue"] == "M268-E002"
    assert payload["upstream"]["M268-A002"]["contract_id"] == "objc3c-part7-async-source-closure/m268-a002-v1"
    assert payload["upstream"]["M268-D002"]["contract_id"] == "objc3c-part7-live-continuation-runtime-integration/m268-d002-v1"
    happy = payload["dynamic"]["happy_path"]
    assert happy["manifest_contracts"]["a002"] == "objc3c-part7-async-source-closure/m268-a002-v1"
    assert happy["manifest_contracts"]["b003"] == "objc3c-part7-async-diagnostics-compatibility-completion/m268-b003-v1"
    assert happy["manifest_contracts"]["c003"] == "objc3c-part7-suspension-autorelease-cleanup-integration/m268-c003-v1"
    assert happy["ir_live_boundary_present"] is True
    assert happy["object_artifact_present"] is True
