"""Stdlib runtime-backed helper acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.expectation_matching import expect_equal
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..paths import ROOT


def check_stdlib_core_runtime_probe_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "stdlib-core-runtime-probe"
    probe = ROOT / "tests" / "tooling" / "runtime" / "stdlib_core_runtime_probe.cpp"
    exe_path = case_dir / "stdlib_core_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "stdlib core runtime probe")
    expect_equal(payload.get("total_call_count"), 17, "stdlib core runtime calls drifted")
    expect_equal(payload.get("revision_call_count"), 2, "stdlib revision helper calls drifted")
    expect_equal(
        payload.get("capability_call_count"),
        2,
        "stdlib capability helper calls drifted",
    )
    expect_equal(payload.get("option_call_count"), 4, "stdlib option helper calls drifted")
    expect_equal(payload.get("count_call_count"), 2, "stdlib count helper calls drifted")
    expect_equal(payload.get("prefix_call_count"), 3, "stdlib prefix helper calls drifted")
    expect_equal(payload.get("map_call_count"), 4, "stdlib map helper calls drifted")
    expect_equal(payload.get("last_result"), 19, "stdlib last helper result drifted")
    return CaseResult(
        case_id="stdlib-core-runtime-probe",
        probe="tests/tooling/runtime/stdlib_core_runtime_probe.cpp",
        fixture="stdlib/modules/objc3.core/module.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "stdlib-core-runtime-backed-helper-probe",
            "runtime_abi": [
                "objc3_runtime_stdlib_core_language_revision_i32",
                "objc3_runtime_stdlib_core_profile_revision_i32",
                "objc3_runtime_stdlib_core_has_capability_i32",
                "objc3_runtime_stdlib_core_option_has_value_i32",
                "objc3_runtime_stdlib_core_option_unwrap_or_i32",
                "objc3_runtime_stdlib_core_count_i32",
                "objc3_runtime_stdlib_core_prefix_count_i32",
                "objc3_runtime_stdlib_core_map_entry_present_i32",
                "objc3_runtime_stdlib_core_map_entry_value_or_i32",
            ],
            "total_call_count": payload.get("total_call_count"),
        },
    )


__all__ = ["check_stdlib_core_runtime_probe_case"]
