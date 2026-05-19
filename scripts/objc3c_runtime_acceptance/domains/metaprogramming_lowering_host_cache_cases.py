"""Metaprogramming lowering host-cache runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_lowering_host_cache_assertions import (
    expect_expansion_lowering_surface,
    expect_host_cache_surfaces,
    expect_lowering_llvm_summary,
    expect_replay_preservation_surface,
    expect_synthesized_emission_surface,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT


def check_metaprogramming_lowering_host_cache_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-lowering-host-cache-surface"
    lowering_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "expansion_lowering_positive.objc3"
    )
    _, lowering_ll_path, lowering_manifest_path = compile_fixture_outputs(
        lowering_fixture, case_dir / "lowering" / "compile"
    )
    lowering_manifest = json.loads(lowering_manifest_path.read_text(encoding="utf-8"))
    semantic_surface = (
        lowering_manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    expansion_lowering_surface = semantic_surface.get(
        "objc_metaprogramming_expansion_and_lowering_contract", {}
    )
    synthesized_emission_surface = semantic_surface.get(
        "objc_metaprogramming_synthesized_ast_and_ir_emission", {}
    )
    replay_preservation_surface = semantic_surface.get(
        "objc_metaprogramming_module_interface_and_replay_preservation", {}
    )
    expect_expansion_lowering_surface(expansion_lowering_surface)
    expect_synthesized_emission_surface(
        synthesized_emission_surface, expansion_lowering_surface
    )
    expect_replay_preservation_surface(
        replay_preservation_surface,
        expansion_lowering_surface,
        synthesized_emission_surface,
    )
    lowering_ll = lowering_ll_path.read_text(encoding="utf-8")
    expect_lowering_llvm_summary(lowering_ll)

    host_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_provider.objc3"
    )
    _, _, host_manifest_path = compile_fixture_outputs(
        host_fixture, case_dir / "host-cache" / "compile"
    )
    host_cache_path = (
        case_dir / "host-cache" / "compile" / "module.metaprogramming-macro-host-cache.json"
    )
    runtime_import_path = (
        case_dir / "host-cache" / "compile" / "module.runtime-import-surface.json"
    )
    expect(
        host_cache_path.is_file() and runtime_import_path.is_file(),
        "expected macro host process provider fixture to publish host-cache and runtime import artifacts",
    )
    host_cache_surface = json.loads(host_cache_path.read_text(encoding="utf-8"))
    runtime_import_surface = json.loads(runtime_import_path.read_text(encoding="utf-8"))
    host_cache_import_surface = runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect_host_cache_surfaces(host_cache_surface, host_cache_import_surface)

    return CaseResult(
        case_id="metaprogramming-lowering-host-cache-surface",
        probe="compile-manifest-metaprogramming-lowering-host-cache-surface",
        fixture="tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "lowering_fixture": {
                "fixture": str(lowering_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(lowering_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "llvm_ir": str(lowering_ll_path.relative_to(ROOT)).replace("\\", "/"),
                "lowering_contract_id": expansion_lowering_surface.get("contract_id"),
                "synthesized_contract_id": synthesized_emission_surface.get("contract_id"),
                "replay_contract_id": replay_preservation_surface.get("contract_id"),
            },
            "host_cache_fixture": {
                "fixture": str(host_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(host_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "host_cache_artifact": str(host_cache_path.relative_to(ROOT)).replace("\\", "/"),
                "runtime_import_surface": str(runtime_import_path.relative_to(ROOT)).replace("\\", "/"),
                "host_cache_contract_id": host_cache_surface.get("contract_id"),
            },
        },
    )
