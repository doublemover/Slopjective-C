"""Error-handling linked-runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import ROOT, compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe


_EXPORTED_CASE_NAMES = [
    "check_error_runtime_abi_cleanup_case",
    "check_live_error_runtime_integration_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_error_runtime_abi_cleanup_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-runtime-abi-cleanup"
    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "error_runtime_bridge_helper_probe.cpp"
    )
    exe_path = case_dir / "error_runtime_bridge_helper_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(run_probe(exe_path), "error runtime ABI cleanup probe")
    expected_integer_fields = {
        "status": 0,
        "loaded": 34,
        "bridged_status": 45,
        "bridged_nserror": 77,
        "match_nserror": 1,
        "match_protocol": 1,
        "match_catch_all": 1,
        "store_call_count": 1,
        "load_call_count": 1,
        "status_bridge_call_count": 1,
        "nserror_bridge_call_count": 1,
        "catch_match_call_count": 3,
        "last_stored_error_value": 34,
        "last_loaded_error_value": 34,
        "last_status_bridge_status_value": 5,
        "last_status_bridge_error_value": 45,
        "last_nserror_bridge_error_value": 77,
        "last_catch_match_error_value": 77,
        "last_catch_match_kind": 0,
        "last_catch_match_is_catch_all": 1,
        "last_catch_match_result": 1,
    }
    for field, expected_value in expected_integer_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected error runtime ABI cleanup probe to preserve {field}",
        )
    expect(
        payload.get("last_catch_kind_name") == "unknown",
        "expected error runtime ABI cleanup probe to preserve the last catch-kind label",
    )
    return CaseResult(
        case_id="error-runtime-abi-cleanup",
        probe="tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
        fixture="tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "bridge_state_snapshot_symbol": "objc3_runtime_copy_error_bridge_state_for_testing",
            "store_symbol": "objc3_runtime_store_thrown_error_i32",
            "load_symbol": "objc3_runtime_load_thrown_error_i32",
            "status_bridge_symbol": "objc3_runtime_bridge_status_error_i32",
            "nserror_bridge_symbol": "objc3_runtime_bridge_nserror_error_i32",
            "catch_match_symbol": "objc3_runtime_catch_matches_error_i32",
        },
    )


def check_live_error_runtime_integration_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-error-runtime-integration"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "live_error_runtime_integration_positive.objc3"
    )
    obj_path, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "live_error_runtime_integration_probe.cpp"
    )
    exe_path = case_dir / "live_error_runtime_integration_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "live error runtime integration probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    throws_abi = manifest.get("lowering_error_handling_throws_abi_propagation", {})
    expect(
        payload.get("status") == 0,
        "expected live error runtime integration probe to copy the bridge-state snapshot successfully",
    )
    expected_integer_fields = {
        "rc": 54,
        "store_call_count": 1,
        "load_call_count": 1,
        "status_bridge_call_count": 1,
        "nserror_bridge_call_count": 0,
        "catch_match_call_count": 1,
        "last_stored_error_value": 45,
        "last_loaded_error_value": 45,
        "last_status_bridge_status_value": 5,
        "last_status_bridge_error_value": 45,
        "last_catch_match_kind": 1,
        "last_catch_match_is_catch_all": 0,
        "last_catch_match_result": 1,
    }
    for field, expected_value in expected_integer_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected live error runtime integration probe to preserve {field}",
        )
    expect(
        payload.get("last_catch_kind_name") == "nserror",
        "expected live error runtime integration probe to preserve the NSError catch-kind label",
    )
    expect(
        isinstance(throws_abi, dict)
        and throws_abi.get("contract_id")
        == "objc3c.error_handling.throws.abi.propagation.lowering.v1",
        "expected live error runtime integration fixture to preserve the throws ABI propagation contract",
    )
    expect(
        "ready_for_runtime_execution=true"
        in str(throws_abi.get("replay_key", "")),
        "expected live error runtime integration fixture to preserve runtime execution readiness in the throws ABI replay packet",
    )
    return CaseResult(
        case_id="live-error-runtime-integration",
        probe="tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
        fixture="tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "rc": payload.get("rc"),
            "status": payload.get("status"),
            "last_catch_kind_name": payload.get("last_catch_kind_name"),
            "throws_abi_contract": throws_abi.get("contract_id"),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
