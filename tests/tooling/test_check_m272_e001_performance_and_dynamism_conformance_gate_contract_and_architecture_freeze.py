from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m272_e001_performance_and_dynamism_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m272" / "M272-E001" / "performance_dynamism_conformance_gate_summary.json"


def test_m272_e001_checker_writes_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-part9-performance-and-dynamism-conformance-gate/m272-e001-v1"
    assert payload["mode"] == "m272-e001-performance-and-dynamism-conformance-gate-v1"
    assert payload["next_closeout_issue"] == "M272-E002"
    assert payload["dynamic"]["skipped"] is True
    assert payload["upstream"]["M272-D002"]["contract_id"] == "objc3c-part9-live-dispatch-fast-path-and-cache-integration/m272-d002-v1"
    assert payload["ok"] is True
