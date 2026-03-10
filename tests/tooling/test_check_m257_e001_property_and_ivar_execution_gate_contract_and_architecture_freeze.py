from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-E001" / "property_ivar_execution_gate_summary.json"
CONTRACT_ID = "objc3c-executable-property-ivar-execution-gate/m257-e001-v1"


def test_m257_e001_checker_emits_summary() -> None:
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
    assert payload["next_closeout_issue"] == "M257-E002"
    assert payload["upstream_summaries"]["M257-A002"]["contract_id"] == "objc3c-executable-property-ivar-source-model-completion/m257-a002-v1"
    assert payload["upstream_summaries"]["M257-B003"]["contract_id"] == "objc3c-property-accessor-attribute-interactions/m257-b003-v1"
    assert payload["upstream_summaries"]["M257-C003"]["contract_id"] == "objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1"
    assert payload["upstream_summaries"]["M257-D003"]["contract_id"] == "objc3c-runtime-property-metadata-reflection/m257-d003-v1"
