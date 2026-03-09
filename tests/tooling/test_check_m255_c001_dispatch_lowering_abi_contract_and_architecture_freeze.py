from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m255_c001_dispatch_lowering_abi_contract_and_architecture_freeze.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m255"
    / "M255-C001"
    / "dispatch_lowering_abi_contract_summary.json"
)


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
    assert payload["mode"] == "m255-c001-dispatch-lowering-abi-contract-and-architecture-freeze-v1"
    assert payload["contract_id"] == "objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes"]["positive"]["returncode"] == 0
    surface = payload["dynamic_probes"]["positive"]["surface"]
    assert surface["canonical_runtime_dispatch_symbol"] == "objc3_runtime_dispatch_i32"
    assert surface["compatibility_runtime_dispatch_symbol"] == "objc3_msgsend_i32"
    assert surface["default_lowering_target_symbol"] == "objc3_msgsend_i32"
    assert surface["selector_lookup_symbol"] == "objc3_runtime_lookup_selector"
    assert surface["selector_handle_type"] == "objc3_runtime_selector_handle"
    assert surface["fixed_argument_slot_count"] == 4
    assert surface["runtime_dispatch_parameter_count"] == 6
