"""Metaprogramming executable lowering runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_executable_lowering_assertions import (
    expect_boundary_llvm_summary,
    expect_emission_llvm_summary,
    expect_runtime_boundary_payload,
    expect_synthesized_emission_surface,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe


def check_metaprogramming_executable_lowering_case(
    clangxx: str,
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-executable-lowering"
    emission_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "synthesized_ast_ir_macro_positive.objc3"
    )
    emission_obj_path, emission_ll_path, emission_manifest_path = compile_fixture_outputs(
        emission_fixture, case_dir / "emission" / "compile"
    )
    emission_manifest = json.loads(emission_manifest_path.read_text(encoding="utf-8"))
    synthesized_emission_surface = (
        emission_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_synthesized_ast_and_ir_emission", {})
    )
    expect_synthesized_emission_surface(synthesized_emission_surface)
    emission_ll = emission_ll_path.read_text(encoding="utf-8")
    expect_emission_llvm_summary(emission_ll)

    boundary_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "expansion_host_runtime_boundary_positive.objc3"
    )
    boundary_obj_path, boundary_ll_path, boundary_manifest_path = compile_fixture_outputs(
        boundary_fixture, case_dir / "boundary" / "compile"
    )
    boundary_ll = boundary_ll_path.read_text(encoding="utf-8")
    expect_boundary_llvm_summary(boundary_ll)

    probe = ROOT / "tests" / "tooling" / "runtime" / "expansion_host_runtime_boundary_probe.cpp"
    exe_path = case_dir / "expansion_host_runtime_boundary_probe.exe"
    compile_probe(clangxx, probe, exe_path, [boundary_obj_path])
    payload = parse_key_value_output(
        run_probe(exe_path), "metaprogramming expansion host/runtime boundary probe"
    )
    expect_runtime_boundary_payload(payload)

    return CaseResult(
        case_id="metaprogramming-executable-lowering",
        probe="compile-linked-metaprogramming-runtime-boundary-probe",
        fixture="tests/tooling/fixtures/native/synthesized_ast_ir_macro_positive.objc3",
        claim_class="compile-linked-runtime-probe",
        passed=True,
        summary={
            "emission_fixture": {
                "fixture": str(emission_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(emission_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "llvm_ir": str(emission_ll_path.relative_to(ROOT)).replace("\\", "/"),
                "object": str(emission_obj_path.relative_to(ROOT)).replace("\\", "/"),
                "emitted_runtime_method_list_sites": synthesized_emission_surface.get(
                    "emitted_runtime_method_list_sites"
                ),
            },
            "runtime_boundary_fixture": {
                "fixture": str(boundary_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(boundary_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "llvm_ir": str(boundary_ll_path.relative_to(ROOT)).replace("\\", "/"),
                "object": str(boundary_obj_path.relative_to(ROOT)).replace("\\", "/"),
                "probe": str(probe.relative_to(ROOT)).replace("\\", "/"),
                "probe_exe": str(exe_path.relative_to(ROOT)).replace("\\", "/"),
                "property_runtime_ready": payload.get("property_runtime_ready"),
                "macro_host_execution_ready": payload.get("macro_host_execution_ready"),
            },
        },
    )
