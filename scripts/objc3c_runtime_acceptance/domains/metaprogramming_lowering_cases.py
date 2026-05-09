"""Metaprogramming lowering and cross-module runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    compile_fixture_outputs,
    compile_fixture_with_args,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

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

def check_cross_module_metaprogramming_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-metaprogramming-artifact-preservation"
    provider_fixture = (
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "preservation_provider.objc3"
    )
    consumer_fixture = (
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "preservation_consumer.objc3"
    )

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface_path = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface_path.read_text(encoding="utf-8")
    )
    provider_replay_surface = provider_import_payload.get(
        "objc_metaprogramming_module_interface_and_replay_preservation", {}
    )
    provider_host_cache_surface = provider_import_payload.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect(
        isinstance(provider_replay_surface, dict),
        "expected metaprogramming preservation provider import surface to publish the replay-preservation packet",
    )
    expect(
        provider_replay_surface.get("contract_id")
        == "objc3c.metaprogramming.module.interface.replay.preservation.v1",
        "expected metaprogramming preservation provider import surface to preserve the replay-preservation contract",
    )
    expect(
        provider_replay_surface.get("source_contract_id")
        == "objc3c.metaprogramming.synthesized.ast.ir.emission.v1",
        "expected metaprogramming preservation provider import surface to preserve the synthesized emission source contract",
    )
    expect(
        provider_replay_surface.get("local_derive_method_count") == 1
        and provider_replay_surface.get("local_macro_artifact_count") == 1
        and provider_replay_surface.get("local_interface_property_behavior_artifact_count")
        == 1
        and provider_replay_surface.get(
            "local_implementation_property_behavior_artifact_count"
        )
        == 1
        and provider_replay_surface.get("local_runtime_method_list_count") == 1,
        "expected metaprogramming preservation provider import surface to preserve local metaprogramming artifact counts",
    )
    expect(
        provider_replay_surface.get("runtime_import_artifact_ready") is True
        and provider_replay_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_replay_surface.get("deterministic") is True,
        "expected metaprogramming preservation provider import surface to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        isinstance(provider_replay_surface.get("replay_key"), str)
        and provider_replay_surface.get("replay_key") != "",
        "expected metaprogramming preservation provider import surface to publish a replay key",
    )

    expect(
        isinstance(provider_host_cache_surface, dict),
        "expected metaprogramming preservation provider import surface to publish the host-cache packet",
    )
    expect(
        provider_host_cache_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected metaprogramming preservation provider import surface to preserve the host-cache contract",
    )
    expect(
        provider_host_cache_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected metaprogramming preservation provider import surface to preserve the host runtime boundary contract",
    )
    expect(
        provider_host_cache_surface.get("local_macro_artifact_count") == 1
        and provider_host_cache_surface.get("local_property_behavior_artifact_count")
        == 2
        and provider_host_cache_surface.get("imported_module_count") == 0,
        "expected metaprogramming preservation provider import surface to preserve host-cache local artifact counts",
    )
    expect(
        provider_host_cache_surface.get("runtime_import_artifact_ready") is True
        and provider_host_cache_surface.get("separate_compilation_ready") is True
        and provider_host_cache_surface.get("deterministic") is True,
        "expected metaprogramming preservation provider host-cache packet to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        provider_host_cache_surface.get("metaprogramming_replay_key")
        == provider_replay_surface.get("replay_key"),
        "expected provider host-cache packet to preserve the metaprogramming replay key",
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface_path),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    for field_name, expected_value in (
        (
            "expected_metaprogramming_host_cache_contract_id",
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ),
        (
            "expected_metaprogramming_host_cache_source_contract_id",
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        ),
        (
            "expected_metaprogramming_host_cache_executable_relative_path",
            "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        ),
        (
            "expected_metaprogramming_host_cache_root_relative_path",
            "tmp/artifacts/objc3c-native/cache/metaprogramming",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module metaprogramming link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [provider_import_payload.get("module_name")]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "expected cross-module metaprogramming link plan to preserve host-cache imported module readiness",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module metaprogramming link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "MetaprogrammingPreservationProvider",
        "expected cross-module metaprogramming link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("metaprogramming_macro_host_process_cache_runtime_integration_present", True),
        ("metaprogramming_macro_host_process_cache_runtime_ready", True),
        ("metaprogramming_macro_host_process_cache_separate_compilation_ready", True),
        ("metaprogramming_macro_host_process_cache_deterministic", True),
        (
            "metaprogramming_macro_host_process_cache_contract_id",
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ),
        (
            "metaprogramming_macro_host_process_cache_source_contract_id",
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        ),
        (
            "metaprogramming_macro_host_process_cache_host_executable_relative_path",
            "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        ),
        (
            "metaprogramming_macro_host_process_cache_root_relative_path",
            "tmp/artifacts/objc3c-native/cache/metaprogramming",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported metaprogramming module to preserve {field_name}",
        )
    expect(
        imported_module.get("metaprogramming_macro_host_process_cache_replay_key")
        == provider_host_cache_surface.get("replay_key"),
        "expected imported metaprogramming module to preserve the provider host-cache replay key",
    )
    imported_replay_key = imported_module.get(
        "metaprogramming_macro_host_process_cache_replay_key", ""
    )
    for snippet in (
        "objc_metaprogramming_module_interface_and_replay_preservation",
        "local_derive_method_count=1",
        "local_macro_artifact_count=1",
        "local_interface_property_behavior_artifact_count=1",
        "local_implementation_property_behavior_artifact_count=1",
        "local_runtime_method_list_count=1",
        "emitted_runtime_method_list_sites=1",
    ):
        expect(
            snippet in imported_replay_key,
            f"expected imported metaprogramming replay key to preserve {snippet}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-metaprogramming-artifact-preservation",
        probe=None,
        fixture="tests/tooling/fixtures/native/preservation_provider.objc3",
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": "tests/tooling/fixtures/native/preservation_provider.objc3",
            "consumer_fixture": "tests/tooling/fixtures/native/preservation_consumer.objc3",
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": link_plan.get("local_module", {}).get("module_name"),
            "imported_module_names": link_plan.get(
                "metaprogramming_host_cache_imported_module_names_lexicographic"
            ),
        },
    )
