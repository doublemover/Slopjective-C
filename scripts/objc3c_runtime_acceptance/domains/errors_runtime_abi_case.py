"""Error runtime ABI cleanup linked-probe acceptance case."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe


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
        "bridged_foreign": 99,
        "bridged_foreign_missing_payload": -3902,
        "bridged_foreign_invalid_kind": -3901,
        "match_nserror": 1,
        "match_protocol": 1,
        "match_foreign": 1,
        "match_catch_all": 1,
        "store_call_count": 1,
        "load_call_count": 1,
        "status_bridge_call_count": 1,
        "nserror_bridge_call_count": 1,
        "foreign_exception_bridge_call_count": 3,
        "catch_match_call_count": 4,
        "last_stored_error_value": 34,
        "last_loaded_error_value": 34,
        "last_status_bridge_status_value": 5,
        "last_status_bridge_error_value": 45,
        "last_nserror_bridge_error_value": 77,
        "last_foreign_exception_kind": 9,
        "last_foreign_exception_payload_value": 88,
        "last_foreign_exception_mapped_error_value": 99,
        "last_foreign_exception_bridge_result": -3901,
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
    expect(
        payload.get("last_foreign_exception_kind_name")
        == "unsupported-foreign-exception",
        "expected error runtime ABI cleanup probe to preserve the last foreign-kind label",
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
            "foreign_exception_bridge_symbol": "objc3_runtime_bridge_foreign_exception_error_i32",
            "catch_match_symbol": "objc3_runtime_catch_matches_error_i32",
        },
    )


__all__ = ["check_error_runtime_abi_cleanup_case"]
