from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m253_e001_metadata_emission_gate.py"
SUMMARY = ROOT / "tmp" / "reports" / "m253" / "M253-E001" / "metadata_emission_gate_summary.json"


def test_checker_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-e001-metadata-emission-gate-v1"
    assert payload["contract_id"] == "objc3c-runtime-metadata-emission-gate/m253-e001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert payload["evidence_model"] == "a002-b003-c006-d003-summary-chain"
    assert payload["failure_model"] == "fail-closed-on-upstream-summary-drift"
    assert payload["next_closeout_issue"] == "M253-E002"

    upstream = payload["upstream_evidence"]
    assert upstream["M253-A002"]["ok"] is True
    assert upstream["M253-A002"]["inspection_backend"] == "llvm-direct"
    assert upstream["M253-A002"]["matrix_row_count"] == 9
    assert upstream["M253-B003"]["ok"] is True
    assert upstream["M253-B003"]["host_object_format"] == "coff"
    assert upstream["M253-C006"]["ok"] is True
    assert "M253-C006-CASE-MESSAGE-SEND" in upstream["M253-C006"]["case_ids"]
    assert upstream["M253-D003"]["ok"] is True
    assert "M253-D003-CASE-MULTI-ARCHIVE-FANIN" in upstream["M253-D003"]["case_ids"]
