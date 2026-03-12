from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m265_e001_type_surface_executable_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-E001" / "type_surface_executable_conformance_gate_summary.json"
CONTRACT_ID = "objc3c-type-surface-executable-conformance-gate/m265-e001-v1"


def test_m265_e001_checker_emits_summary() -> None:
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
    assert payload["next_closeout_issue"] == "M265-E002"
    assert payload["upstream_summaries"]["M265-A002"]["contract_id"] == "objc3c-part3-type-source-closure/m265-a002-v1"
    assert payload["upstream_summaries"]["M265-B003"]["contract_id"] == "objc3c-part3-type-semantic-model/m265-b001-v1"
    assert payload["upstream_summaries"]["M265-C003"]["contract_id"] == "objc3c-part3-typed-keypath-artifact-emission/m265-c003-v1"
    assert payload["upstream_summaries"]["M265-D003"]["contract_id"] == "objc3c-part3-cross-module-type-surface-preservation/m265-d003-v1"
    integrated = payload["dynamic"]["integrated_fixture"]
    assert integrated["module_name"] == "optionalsFrontend"
    assert integrated["backend"] == "llvm-direct"
    assert integrated["type_surface"]["optional_binding_sites"] == 2
    assert integrated["type_surface"]["optional_send_sites"] == 1
    assert integrated["type_surface"]["typed_keypath_literal_sites"] == 1
