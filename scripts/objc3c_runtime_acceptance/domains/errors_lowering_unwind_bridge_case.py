"""Error lowering unwind bridge helper surface acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT


def check_error_lowering_unwind_bridge_helper_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-lowering-unwind-bridge-helper-surface"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_out_abi_positive.objc3"
    )
    _, ll_path, manifest_path = compile_live_error_runtime_fixture_outputs(
        fixture, case_dir / "compile"
    )
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    expect(
        isinstance(throws_abi, dict)
        and throws_abi.get("contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected error lowering fixture to publish the throws ABI propagation lowering surface",
    )
    helper_calls = {
        "store": "call void @objc3_runtime_store_thrown_error_i32" in ll_text,
        "load": "call i32 @objc3_runtime_load_thrown_error_i32" in ll_text,
        "status_bridge": "call i32 @objc3_runtime_bridge_status_error_i32"
        in ll_text,
        "catch_match": "call i32 @objc3_runtime_catch_matches_error_i32"
        in ll_text,
    }
    expect(
        all(helper_calls.values()),
        "expected error lowering fixture to emit runtime bridge helper call sites",
    )
    return CaseResult(
        case_id="error-lowering-unwind-bridge-helper-surface",
        probe="compile-artifact-llvm-helper-lowering",
        fixture="tests/tooling/fixtures/native/error_out_abi_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "throws_abi_contract": throws_abi.get("contract_id"),
            "helper_calls": helper_calls,
        },
    )


__all__ = ["check_error_lowering_unwind_bridge_helper_surface_case"]
