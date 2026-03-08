from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m254_e001_startup_registration_gate.py"
SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-E001" / "startup_registration_gate_summary.json"


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
    assert payload["mode"] == "m254-e001-startup-registration-gate-v1"
    assert payload["contract_id"] == "objc3c-runtime-startup-registration-gate/m254-e001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert payload["evidence_model"] == "a002-b002-c003-d003-d004-summary-chain"
    assert payload["failure_model"] == "fail-closed-on-bootstrap-evidence-drift"
    assert payload["next_closeout_issue"] == "M254-E002"

    upstream = payload["upstream_evidence"]
    assert upstream["M254-A002"]["ok"] is True
    assert upstream["M254-A002"]["backend"] == "llvm-direct"
    assert upstream["M254-A002"]["runtime_archive"] == "artifacts/lib/objc3_runtime.lib"
    assert upstream["M254-B002"]["ok"] is True
    assert upstream["M254-B002"]["duplicate_status"] == -2
    assert upstream["M254-B002"]["out_of_order_status"] == -3
    assert upstream["M254-C003"]["ok"] is True
    assert "metadata-library" in upstream["M254-C003"]["case_ids"]
    assert "category-library" in upstream["M254-C003"]["case_ids"]
    assert upstream["M254-D003"]["ok"] is True
    assert "metadata-library" in upstream["M254-D003"]["case_ids"]
    assert upstream["M254-D004"]["ok"] is True
    assert upstream["M254-D004"]["smoke_status"] == "PASS"
    assert upstream["M254-D004"]["compile_wrapper"] == "scripts/objc3c_native_compile.ps1"
