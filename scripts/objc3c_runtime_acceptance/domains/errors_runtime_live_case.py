"""Live error runtime integration linked-probe acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_live_error_runtime_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe


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
    obj_path, _, manifest_path = compile_live_error_runtime_fixture_outputs(
        fixture, case_dir / "compile"
    )
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
        "load_call_count": 2,
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


__all__ = ["check_live_error_runtime_integration_case"]
