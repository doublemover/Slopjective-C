"""Executable throw/catch cleanup lowering acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.checksums import replay_key_counter
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT


def check_executable_throw_catch_cleanup_lowering_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "executable-throw-catch-cleanup-lowering"
    fixture_relative_path = (
        "tests/tooling/fixtures/native/error_arc_cleanup_bridge_positive.objc3"
    )
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_arc_cleanup_bridge_positive.objc3"
    )
    _, ll_path, manifest_path = compile_live_error_runtime_fixture_outputs(
        fixture, case_dir / "compile", ["-fobjc-arc"]
    )
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    result_replay = manifest.get(
        "lowering_error_handling_result_and_bridging_artifact_replay", {}
    )
    ns_error_bridging = manifest.get("lowering_ns_error_bridging", {})
    unwind_cleanup = manifest.get("lowering_unwind_cleanup", {})
    unwind_cleanup_replay_key = str(unwind_cleanup.get("replay_key", ""))
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
    helper_calls = {
        "store": "call void @objc3_runtime_store_thrown_error_i32" in ll_text,
        "load": "call i32 @objc3_runtime_load_thrown_error_i32" in ll_text,
        "status_bridge": "call i32 @objc3_runtime_bridge_status_error_i32"
        in ll_text,
        "catch_match": "call i32 @objc3_runtime_catch_matches_error_i32"
        in ll_text,
    }
    cleanup_calls = {
        "cleanup_function": "call void @cleanup_release_temp" in ll_text,
        "defer_marker": "call void @cleanup_release_temp(i32 6)" in ll_text,
        "resource_cleanup": "call void @cleanup_scope_close_fd" in ll_text,
        "arc_release": "call i32 @objc3_runtime_release_i32" in ll_text,
        "autorelease": "call i32 @objc3_runtime_autorelease_i32" in ll_text,
        "autoreleasepool_push": "call void @objc3_runtime_push_autoreleasepool_scope"
        in ll_text,
        "autoreleasepool_pop": "call void @objc3_runtime_pop_autoreleasepool_scope"
        in ll_text,
    }
    expect(
        all(helper_calls.values()),
        "expected executable error lowering fixture to emit all throw/catch bridge helpers",
    )
    expect(
        all(cleanup_calls.values()),
        "expected executable error lowering fixture to couple ARC cleanup, @cleanup, @resource, and autoreleasepool helpers with thrown bridge flow",
    )
    try_failure_index = ll_text.find("try_fail_")
    catch_dispatch_index = ll_text.find("do_catch_dispatch_", try_failure_index)
    expect(
        try_failure_index >= 0 and catch_dispatch_index > try_failure_index,
        "expected executable error lowering fixture to expose try failure cleanup before catch dispatch",
    )
    failure_cleanup_segment = ll_text[try_failure_index:catch_dispatch_index]
    resource_cleanup_index = failure_cleanup_segment.find(
        "call void @cleanup_scope_close_fd"
    )
    defer_cleanup_index = failure_cleanup_segment.find(
        "call void @cleanup_release_temp(i32 6)"
    )
    expect(
        0 <= resource_cleanup_index < defer_cleanup_index,
        "expected thrown-path cleanup lowering to honor LIFO order for a resource registered after a defer before catch dispatch",
    )
    cleanup_counts = {
        "unwind_cleanup_sites": replay_key_counter(
            unwind_cleanup_replay_key, "unwind_cleanup_sites"
        ),
        "unwind_edge_sites": replay_key_counter(
            unwind_cleanup_replay_key, "unwind_edge_sites"
        ),
        "cleanup_emit_sites": replay_key_counter(
            unwind_cleanup_replay_key, "cleanup_emit_sites"
        ),
        "cleanup_scope_sites": replay_key_counter(
            unwind_cleanup_replay_key, "cleanup_scope_sites"
        ),
        "cleanup_resume_sites": replay_key_counter(
            unwind_cleanup_replay_key, "cleanup_resume_sites"
        ),
    }
    required_positive_cleanup_fields = (
        "unwind_cleanup_sites",
        "unwind_edge_sites",
        "cleanup_emit_sites",
        "cleanup_scope_sites",
    )
    for field in required_positive_cleanup_fields:
        value = cleanup_counts.get(field)
        expect(
            isinstance(value, int) and value > 0,
            f"expected executable error lowering fixture to publish positive {field}",
        )
    expect(
        isinstance(cleanup_counts["cleanup_resume_sites"], int)
        and cleanup_counts["cleanup_resume_sites"] >= 0,
        "expected executable error lowering fixture to publish cleanup_resume_sites",
    )
    return CaseResult(
        case_id="executable-throw-catch-cleanup-lowering",
        probe="compile-artifact-llvm-helper-lowering",
        fixture=fixture_relative_path,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_abi_contract": throws_abi.get("contract_id"),
            "result_replay_contract": result_replay.get("contract_id"),
            "ns_error_bridging_contract": ns_error_bridging.get("lane_contract"),
            "unwind_cleanup_contract": unwind_cleanup.get("lane_contract"),
            "helper_calls": helper_calls,
            "cleanup_calls": cleanup_calls,
            "thrown_path_resource_before_defer": True,
            "cleanup_counts": cleanup_counts,
        },
    )


__all__ = ["check_executable_throw_catch_cleanup_lowering_case"]
