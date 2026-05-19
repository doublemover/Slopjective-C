"""Executable throw/catch cleanup lowering acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT


def check_executable_throw_catch_cleanup_lowering_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "executable-throw-catch-cleanup-lowering"
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
    result_replay = manifest.get("error_handling_result_and_bridging_artifact_replay", {})
    ns_error_bridging = manifest.get("ns_error_bridging_lowering_surface", {})
    unwind_cleanup = manifest.get("unwind_cleanup_lowering_surface", {})
    expect(
        isinstance(throws_abi, dict)
        and throws_abi.get("contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected executable error lowering fixture to preserve the throws ABI propagation contract",
    )
    expect(
        isinstance(result_replay, dict)
        and result_replay.get("contract_id")
        == "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        "expected executable error lowering fixture to preserve the error result/bridging replay contract",
    )
    expect(
        isinstance(ns_error_bridging, dict)
        and ns_error_bridging.get("lane_contract")
        == "objc3c.ns.error.bridging.lowering.v1",
        "expected executable error lowering fixture to preserve the NSError bridging lowering contract",
    )
    expect(
        isinstance(unwind_cleanup, dict)
        and unwind_cleanup.get("lane_contract") == "objc3c.unwind.cleanup.lowering.v1",
        "expected executable error lowering fixture to preserve the unwind cleanup lowering contract",
    )
    return CaseResult(
        case_id="executable-throw-catch-cleanup-lowering",
        probe="compile-artifact-llvm-helper-lowering",
        fixture="tests/tooling/fixtures/native/error_out_abi_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_abi_contract": throws_abi.get("contract_id"),
            "result_replay_contract": result_replay.get("contract_id"),
            "ns_error_bridging_contract": ns_error_bridging.get("lane_contract"),
            "unwind_cleanup_contract": unwind_cleanup.get("lane_contract"),
            "helper_calls": {
                "store": "objc3_runtime_store_thrown_error_i32" in ll_text,
                "load": "objc3_runtime_load_thrown_error_i32" in ll_text,
                "status_bridge": "objc3_runtime_bridge_status_error_i32" in ll_text,
                "catch_match": "objc3_runtime_catch_matches_error_i32" in ll_text,
            },
        },
    )


__all__ = ["check_executable_throw_catch_cleanup_lowering_case"]
