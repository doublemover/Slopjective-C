from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-E001" / "live_dispatch_gate_summary.json"


def test_m255_e001_checker() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ok"] is True
    assert summary["contract_id"] == "objc3c-runtime-live-dispatch-gate/m255-e001-v1"
    assert summary["evidence_model"] == "a002-b003-c004-d004-summary-chain"
    assert summary["upstream_evidence"]["m255_c004"]["canonical_symbol"] == "objc3_runtime_dispatch_i32"
    assert summary["upstream_evidence"]["m255_c004"]["compatibility_call_count_zero"] is True
    assert summary["upstream_evidence"]["m255_d004"]["instance_first"] == 33
    assert summary["upstream_evidence"]["m255_d004"]["class_self"] == 44
    assert summary["upstream_evidence"]["m255_d004"]["fallback_protocol_probe_count"] == 2
