from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m274_b001_part11_interop_semantic_model_contract_and_architecture_freeze.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-B001" / "part11_interop_semantic_model_summary.test.json"
CONTRACT_ID = "objc3c-part11-interop-semantic-model/m274-b001-v1"


def test_m274_b001_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(TEST_SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is True
    packet = payload["dynamic_summary"]["objc_part11_interop_semantic_model"]
    assert packet["foreign_callable_sites"] == 2
    assert packet["import_module_annotation_sites"] == 1
    assert packet["imported_module_name_sites"] == 1
    assert packet["swift_name_annotation_sites"] == 1
    assert packet["swift_private_annotation_sites"] == 1
    assert packet["cpp_name_annotation_sites"] == 1
    assert packet["header_name_annotation_sites"] == 1
    assert packet["named_annotation_payload_sites"] == 3
    assert packet["bridge_callable_sites"] == 0
    assert packet["async_executor_affinity_sites"] == 0
    assert packet["actor_hazard_sites"] == 0
    assert packet["ffi_abi_lowering_deferred"] is True
    assert packet["runtime_bridge_generation_deferred"] is True
    assert packet["deterministic"] is True
