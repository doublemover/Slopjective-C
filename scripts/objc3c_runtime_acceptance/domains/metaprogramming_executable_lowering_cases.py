"""Metaprogramming executable lowering runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import ROOT


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
    expect(
        synthesized_emission_surface.get("contract_id")
        == "objc3c.metaprogramming.synthesized.ast.ir.emission.v1",
        "expected synthesized AST/IR macro fixture to preserve the synthesized AST/IR emission contract",
    )
    expect(
        synthesized_emission_surface.get("emitted_derive_method_sites") == 1
        and synthesized_emission_surface.get("emitted_macro_artifact_sites") == 1
        and synthesized_emission_surface.get("emitted_property_behavior_artifact_sites")
        == 2
        and synthesized_emission_surface.get("emitted_global_artifact_sites") == 4
        and synthesized_emission_surface.get("emitted_runtime_method_list_sites") == 1
        and synthesized_emission_surface.get("guard_blocked_sites") == 0
        and synthesized_emission_surface.get("contract_violation_sites") == 0
        and synthesized_emission_surface.get("deterministic_handoff") is True
        and synthesized_emission_surface.get("ready_for_ir_emission") is True,
        "expected synthesized AST/IR macro fixture to preserve executable metaprogramming lowering counts and readiness",
    )
    emission_ll = emission_ll_path.read_text(encoding="utf-8")
    expect(
        "metaprogramming_synthesized_ast_and_ir_emission" in emission_ll
        and "emitted_derive_method_sites=1" in emission_ll
        and "emitted_macro_artifact_sites=1" in emission_ll
        and "emitted_property_behavior_artifact_sites=2" in emission_ll,
        "expected synthesized AST/IR macro fixture LLVM IR to preserve the executable lowering summary",
    )

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
    expect(
        "metaprogramming_expansion_host_runtime_boundary" in boundary_ll
        and "property_runtime_ready=true" in boundary_ll
        and "macro_host_execution_ready=false" in boundary_ll
        and "runtime_package_loader_ready=false" in boundary_ll,
        "expected expansion host/runtime boundary fixture LLVM IR to preserve the deferred macro-execution boundary summary",
    )

    probe = ROOT / "tests" / "tooling" / "runtime" / "expansion_host_runtime_boundary_probe.cpp"
    exe_path = case_dir / "expansion_host_runtime_boundary_probe.exe"
    compile_probe(clangxx, probe, exe_path, [boundary_obj_path])
    payload = parse_key_value_output(
        run_probe(exe_path), "metaprogramming expansion host/runtime boundary probe"
    )
    expect(
        payload.get("copy_status") == 0
        and payload.get("property_runtime_ready") == 1
        and payload.get("macro_host_execution_ready") == 0
        and payload.get("macro_host_process_launch_ready") == 0
        and payload.get("runtime_package_loader_ready") == 0
        and payload.get("deterministic") == 1,
        "expected runtime boundary probe to preserve executable lowering readiness while macro host execution remains deferred",
    )
    expect(
        payload.get("runtime_support_library_archive_relative_path")
        == "artifacts/lib/objc3_runtime.lib",
        "expected runtime boundary probe to preserve the runtime support library archive path",
    )
    expect(
        payload.get("property_behavior_runtime_model")
        == "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks",
        "expected runtime boundary probe to preserve the property behavior runtime model",
    )
    expect(
        payload.get("macro_expansion_host_model")
        == "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed",
        "expected runtime boundary probe to preserve the macro expansion host model",
    )
    expect(
        payload.get("fail_closed_model")
        == "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet",
        "expected runtime boundary probe to preserve the fail-closed runtime model",
    )

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
