from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m274_b002_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-B002" / "part11_c_and_objc_runtime_parity_semantics_summary.test.json"
CONTRACT_ID = "objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1"


def test_m274_b002_checker_emits_static_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(TEST_SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr

    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_part11_c_and_objc_runtime_parity_semantics"
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False

    static_contract = payload["static_contract"]
    assert static_contract["docs/contracts/m274_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze_b002_expectations.md"]["checks_passed"] == static_contract["docs/contracts/m274_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze_b002_expectations.md"]["checks_total"]
    assert static_contract["spec/planning/compiler/m274/m274_b002_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze_packet.md"]["checks_passed"] == static_contract["spec/planning/compiler/m274/m274_b002_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze_packet.md"]["checks_total"]
    assert static_contract["tests/tooling/fixtures/native/m274_b002_part11_c_and_objc_runtime_parity_semantics_positive.objc3"]["checks_passed"] == static_contract["tests/tooling/fixtures/native/m274_b002_part11_c_and_objc_runtime_parity_semantics_positive.objc3"]["checks_total"]

