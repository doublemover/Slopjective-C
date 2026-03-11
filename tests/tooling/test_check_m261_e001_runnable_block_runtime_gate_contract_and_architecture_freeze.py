from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-E001" / "block_runtime_gate_summary.json"
CONTRACT_ID = "objc3c-runnable-block-runtime-gate/m261-e001-v1"


def test_m261_e001_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT)],
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
    assert payload["next_closeout_issue"] == "M261-E002"
    assert payload["upstream_summaries"]["M261-A003"]["contract_id"] == "objc3c-executable-block-source-storage-annotation/m261-a003-v1"
    assert payload["upstream_summaries"]["M261-B003"]["contract_id"] == "objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1"
    assert payload["upstream_summaries"]["M261-C004"]["contract_id"] == "objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1"
    assert payload["upstream_summaries"]["M261-D003"]["contract_id"] == "objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1"
