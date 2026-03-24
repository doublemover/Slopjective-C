from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m274_c001_interop_lowering_and_abi_contract_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-C001" / "interop_lowering_and_abi_contract_summary.test.json"
CONTRACT_ID = "objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1"
RUNNER = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"


def test_m274_c001_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr

    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_part11_interop_lowering_and_abi_contract"
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False

    static_contract = payload["dynamic_summary"]
    assert static_contract["skipped"] is True


def test_m274_c001_checker_emits_dynamic_summary() -> None:
    assert RUNNER.exists(), RUNNER
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--runner-exe", str(RUNNER), "--summary-out", str(SUMMARY)],
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
    assert payload["dynamic_probes_executed"] is True

    dynamic_contract = payload["dynamic_summary"]
    surface = dynamic_contract["interop_lowering_contract"]
    assert surface["contract_id"] == CONTRACT_ID
    assert surface["semantic_contract_id"] == "objc3c-part11-interop-semantic-model/m274-b001-v1"
    assert surface["runtime_parity_contract_id"] == "objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1"
    assert surface["cpp_interaction_contract_id"] == "objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1"
    assert surface["swift_isolation_contract_id"] == "objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1"
    assert surface["preservation_contract_id"] == "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1"
    assert surface["ready_for_ir_emission"] is True
    assert surface["deterministic_handoff"] is True