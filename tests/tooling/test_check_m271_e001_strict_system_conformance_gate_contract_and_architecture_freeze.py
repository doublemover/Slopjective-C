from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m271_e001_strict_system_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m271" / "M271-E001" / "strict_system_conformance_gate_summary.json"


def test_m271_e001_checker_writes_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-strict-system-conformance-gate/m271-e001-v1"
    assert payload["mode"] == "m271-e001-strict-system-conformance-gate-v1"
    assert payload["next_closeout_issue"] == "M271-E002"
    assert payload["dynamic"]["skipped"] is True
    assert payload["upstream"]["M271-D002"]["contract_id"] == "objc3c-part8-live-cleanup-retainable-runtime-integration/m271-d002-v1"
    assert payload["ok"] is True
