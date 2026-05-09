"""Metaprogramming lowering host-cache runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs

from ..core import ROOT


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
    expect(
        expansion_lowering_surface.get("contract_id")
        == "objc3c.metaprogramming.expansion.lowering.contract.v1",
        "expected expansion lowering fixture to preserve the metaprogramming lowering contract",
    )
    expect(
        expansion_lowering_surface.get("derive_contract_id")
        == "objc3c.metaprogramming.derive.expansion.inventory.v1"
        and expansion_lowering_surface.get("macro_contract_id")
        == "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1"
        and expansion_lowering_surface.get("property_legality_contract_id")
        == "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1",
        "expected expansion lowering fixture to preserve the derive/macro/property legality dependencies",
    )
    expect(
        expansion_lowering_surface.get("derived_selector_artifact_sites") == 1
        and expansion_lowering_surface.get("macro_replay_visible_sites") == 1
        and expansion_lowering_surface.get("property_behavior_sites") == 2
        and expansion_lowering_surface.get("guard_blocked_sites") == 0
        and expansion_lowering_surface.get("contract_violation_sites") == 0
        and expansion_lowering_surface.get("deterministic_handoff") is True
        and expansion_lowering_surface.get("ready_for_ir_emission") is True,
        "expected expansion lowering fixture to preserve deterministic lowering counts and readiness",
    )

    expect(
        synthesized_emission_surface.get("contract_id")
        == "objc3c.metaprogramming.synthesized.ast.ir.emission.v1",
        "expected expansion lowering fixture to preserve the synthesized AST/IR emission contract",
    )
    expect(
        synthesized_emission_surface.get("dependency_contract_id")
        == expansion_lowering_surface.get("contract_id"),
        "expected synthesized AST/IR emission to depend on the lowering contract",
    )
    expect(
        synthesized_emission_surface.get("emitted_derive_method_sites") == 1
        and synthesized_emission_surface.get("emitted_macro_artifact_sites") == 1
        and synthesized_emission_surface.get("emitted_property_behavior_artifact_sites")
        == 2
        and synthesized_emission_surface.get("emitted_runtime_method_list_sites") == 1
        and synthesized_emission_surface.get("guard_blocked_sites") == 0
        and synthesized_emission_surface.get("contract_violation_sites") == 0
        and synthesized_emission_surface.get("deterministic_handoff") is True
        and synthesized_emission_surface.get("ready_for_ir_emission") is True,
        "expected synthesized AST/IR emission to preserve executable artifact counts and readiness",
    )
    expect(
        synthesized_emission_surface.get("dependency_replay_key")
        == expansion_lowering_surface.get("replay_key"),
        "expected synthesized AST/IR emission to preserve the lowering replay key",
    )

    expect(
        replay_preservation_surface.get("contract_id")
        == "objc3c.metaprogramming.module.interface.replay.preservation.v1",
        "expected expansion lowering fixture to preserve the module replay preservation contract",
    )
    expect(
        replay_preservation_surface.get("source_contract_id")
        == synthesized_emission_surface.get("contract_id"),
        "expected replay preservation to depend on the synthesized emission contract",
    )
    expect(
        replay_preservation_surface.get("local_derive_method_count") == 1
        and replay_preservation_surface.get("local_macro_artifact_count") == 1
        and replay_preservation_surface.get("local_interface_property_behavior_artifact_count")
        == 1
        and replay_preservation_surface.get(
            "local_implementation_property_behavior_artifact_count"
        )
        == 1
        and replay_preservation_surface.get("local_runtime_method_list_count") == 1
        and replay_preservation_surface.get("runtime_import_artifact_ready") is True
        and replay_preservation_surface.get("separate_compilation_preservation_ready")
        is True
        and replay_preservation_surface.get("deterministic") is True,
        "expected replay preservation to preserve local artifact counts and readiness",
    )
    expect(
        replay_preservation_surface.get("expansion_lowering_replay_key")
        == expansion_lowering_surface.get("replay_key")
        and replay_preservation_surface.get("synthesized_emission_replay_key")
        == synthesized_emission_surface.get("replay_key"),
        "expected replay preservation to preserve lowering and synthesized emission replay keys",
    )
    lowering_ll = lowering_ll_path.read_text(encoding="utf-8")
    expect(
        "metaprogramming_synthesized_ast_and_ir_emission" in lowering_ll,
        "expected lowering fixture LLVM IR to preserve the metaprogramming synthesized emission summary",
    )

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
    expect(
        host_cache_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        and host_cache_import_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected host-cache artifact and runtime import surface to preserve the host-cache integration contract",
    )
    expect(
        host_cache_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1"
        and host_cache_import_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected host-cache artifact and runtime import surface to preserve the host runtime boundary contract",
    )
    expect(
        host_cache_surface.get("host_executable_relative_path")
        == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
        and host_cache_surface.get("cache_root_relative_path")
        == "tmp/artifacts/objc3c-native/cache/metaprogramming"
        and host_cache_import_surface.get("host_executable_relative_path")
        == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
        and host_cache_import_surface.get("cache_root_relative_path")
        == "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "expected host-cache artifact and runtime import surface to preserve compatibility paths",
    )
    expect(
        host_cache_import_surface.get("runtime_import_artifact_ready") is True
        and host_cache_import_surface.get("separate_compilation_ready") is True
        and host_cache_import_surface.get("deterministic") is True
        and host_cache_surface.get("deterministic") is True,
        "expected host-cache artifact and runtime import surface to preserve deterministic compatibility readiness",
    )
    expect(
        host_cache_import_surface.get("replay_key") == host_cache_surface.get("replay_key"),
        "expected host-cache artifact and runtime import surface to preserve the same replay key",
    )

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
