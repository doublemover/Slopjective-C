"""Block/ARC runtime ABI linked-probe acceptance case."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.block_arc_runtime_abi_assertions import (
    assert_block_arc_runtime_abi_payload,
)
from objc3c_runtime_acceptance.domains.block_arc_runtime_shared import (
    compile_run_json_probe,
)
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_block_arc import BLOCK_ARC_RUNTIME_ABI_PROBE


def check_block_arc_runtime_abi_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-arc-runtime-abi"
    probe = ROOT / Path(BLOCK_ARC_RUNTIME_ABI_PROBE)
    exe_path = case_dir / "block_arc_runtime_abi_probe.exe"
    payload = compile_run_json_probe(
        clangxx,
        probe,
        exe_path,
        "block ARC runtime ABI probe",
    )
    handle = assert_block_arc_runtime_abi_payload(payload)

    return CaseResult(
        case_id="block-arc-runtime-abi",
        probe=BLOCK_ARC_RUNTIME_ABI_PROBE,
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "handle": handle,
            "invoke_result": payload.get("invoke_result"),
            "block_promote_call_count": payload.get("block_promote_call_count"),
            "block_invoke_call_count": payload.get("block_invoke_call_count"),
            "retain_call_count": payload.get("retain_call_count"),
            "release_call_count": payload.get("release_call_count"),
            "autorelease_call_count": payload.get("autorelease_call_count"),
        },
    )


__all__ = ["check_block_arc_runtime_abi_case"]
