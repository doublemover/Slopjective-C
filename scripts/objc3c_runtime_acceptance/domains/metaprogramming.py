"""Metaprogramming runtime acceptance domain."""

from __future__ import annotations

import json
import shutil
from datetime import datetime
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    NegativeDiagnosticExpectation,
    compile_fixture_outputs,
    compile_fixture_with_args,
    compile_negative_diagnostic_batch,
    run_fixture_compile,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    ROOT,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from ..progress import repo_display_path

RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.source.surface.v1"
)


RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.package.provenance.source.surface.v1"
)


RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.semantics.surface.v1"
)


RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.lowering.host.cache.surface.v1"
)


RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.metaprogramming.artifact.preservation.surface.v1"
)


RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.runtime.abi.cache.surface.v1"
)


RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.metaprogramming.cache.runtime.integration.implementation.surface.v1"
)


PRIVATE_METAPROGRAMMING_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing",
    "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing",
]


METAPROGRAMMING_RUNTIME_ABI_BOUNDARY_MODEL = (
    "private-metaprogramming-boundary-and-host-cache-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header"
)


METAPROGRAMMING_EXPANSION_RUNTIME_MODEL = (
    "property-behavior-runtime-support-is-live-while-macro-host-execution-runtime-process-launch-and-package-loading-remain-fail-closed-on-the-expansion-boundary-snapshot"
)


METAPROGRAMMING_HOST_CACHE_RUNTIME_MODEL = (
    "deterministic-host-process-launch-cache-root-selection-and-replay-key-compatible-cache-materialization-stay-on-bootstrap-internal-snapshots-and-runtime-import-surfaces"
)


METAPROGRAMMING_RUNTIME_FAIL_CLOSED_MODEL = (
    "public-runtime-header-remains-registration-lookup-dispatch-only-and-runtime-package-loading-stays-disabled-until-deliberate-metaprogramming-runtime-abi-widening"
)


_EXPORTED_CASE_NAMES = [
    "build_runtime_metaprogramming_source_surface",
    "build_runtime_metaprogramming_package_provenance_source_surface",
    "build_runtime_metaprogramming_semantics_surface",
    "build_runtime_metaprogramming_lowering_host_cache_surface",
    "build_runtime_cross_module_metaprogramming_artifact_preservation_surface",
    "build_runtime_metaprogramming_runtime_abi_cache_surface",
    "build_runtime_metaprogramming_cache_runtime_integration_implementation_surface",
    "check_metaprogramming_source_surface_case",
    "check_metaprogramming_package_provenance_source_surface_case",
    "check_metaprogramming_semantics_case",
    "check_metaprogramming_derive_property_behavior_semantics_case",
    "check_metaprogramming_macro_safety_cache_diagnostics_case",
    "check_metaprogramming_lowering_host_cache_surface_case",
    "check_metaprogramming_executable_lowering_case",
    "check_cross_module_metaprogramming_artifact_preservation_case",
    "check_metaprogramming_runtime_abi_cache_surface_case",
    "check_live_metaprogramming_cache_runtime_integration_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def remove_metaprogramming_cache_entry_from_artifact(artifact: dict[str, Any]) -> bool:
    relative_entry = artifact.get("cache_entry_relative_path")
    if not isinstance(relative_entry, str) or relative_entry == "":
        return False
    cache_entry = ROOT / Path(relative_entry)
    allowed_root = ROOT / "tmp" / "artifacts" / "objc3c-native" / "cache" / "metaprogramming"
    try:
        cache_entry.resolve().relative_to(allowed_root.resolve())
    except ValueError:
        return False
    if cache_entry.is_dir():
        shutil.rmtree(cache_entry)
        return True
    if cache_entry.is_file():
        cache_entry.unlink()
        return True
    return False


def build_runtime_metaprogramming_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"metaprogramming-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.metaprogramming.metaprogramming.source.closure.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/token/objc3_token_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3InterfaceDecl.objc_derive_declared",
            "Objc3InterfaceDecl.objc_derive_name",
            "Objc3FunctionDecl.objc_macro_declared",
            "Objc3FunctionDecl.objc_macro_name",
            "Objc3PropertyDecl.property_behavior_name",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_derive_macro_property_behavior_source_closure",
        ],
        "source_surface_model": (
            "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-before-semantic-expansion-lowering-or-runtime-materialization"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        ],
        "explicit_non_goals": [
            "no-runnable-macro-execution-claim",
            "no-derived-method-body-materialization-claim",
            "no-property-behavior-runtime-hook-claim",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_metaprogramming_package_provenance_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"metaprogramming-package-provenance-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/token/objc3_token_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3FunctionDecl.objc_macro_package_declared",
            "Objc3FunctionDecl.objc_macro_package_name",
            "Objc3FunctionDecl.objc_macro_provenance_declared",
            "Objc3FunctionDecl.objc_macro_provenance_name",
            "Objc3PropertyDecl.property_behavior_name",
            "Objc3PropertyDecl.executable_synthesized_binding_symbol",
            "Objc3PropertyDecl.effective_getter_selector",
            "Objc3PropertyDecl.effective_setter_selector",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_package_and_provenance_source_completion",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion",
        ],
        "source_surface_model": (
            "macro-package-macro-provenance-and-property-behavior-source-completion-freezes-expansion-visible-and-synthesized-declaration-state-before-semantic-expansion-lowering-or-runtime-materialization"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_behavior_source_completion_positive.objc3",
        ],
        "explicit_non_goals": [
            "no-macro-sandbox-execution-claim",
            "no-property-behavior-runtime-hook-claim",
            "no-executable-synthesized-declaration-materialization-claim",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_metaprogramming_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "metaprogramming-semantics",
            "metaprogramming-derive-property-behavior-semantics",
            "metaprogramming-macro-safety-cache-diagnostics",
        }
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
        "semantic_contract_ids": [
            "objc3c.metaprogramming.expansion.behavior.semantic.model.v1",
            "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1",
        ],
        "source_dependency_contract_ids": [
            "objc3c.metaprogramming.metaprogramming.source.closure.v1",
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ],
        "cache_runtime_contract_ids": [
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "semantic_surface_model": (
            "derive-macro-package-provenance-property-behavior-and-macro-safety-packets-share-one-deterministic-sema-boundary-before-lowering-runtime-host-cache-integration-or-runtime-hooks"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_behavior_semantic_model_positive.objc3",
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_metadata.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_orphan_metadata.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_invalid_package.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_invalid_provenance.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_nonpure.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_method_topology.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
        ],
        "requires_real_compile_output": True,
    }


def build_runtime_metaprogramming_lowering_host_cache_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "metaprogramming-macro-safety-cache-diagnostics",
            "metaprogramming-lowering-host-cache-surface",
            "metaprogramming-executable-lowering",
        }
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "runtime_import_surface_artifact": "<emit-prefix>.runtime-import-surface.json",
        "host_cache_artifact": "<emit-prefix>.metaprogramming-macro-host-cache.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "runtime_metaprogramming_semantics_surface_contract_id": (
            RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "expansion_lowering_contract_id": (
            "objc3c.metaprogramming.expansion.lowering.contract.v1"
        ),
        "synthesized_ast_ir_emission_contract_id": (
            "objc3c.metaprogramming.synthesized.ast.ir.emission.v1"
        ),
        "module_interface_replay_preservation_contract_id": (
            "objc3c.metaprogramming.module.interface.replay.preservation.v1"
        ),
        "macro_host_process_cache_runtime_integration_contract_id": (
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        ),
        "host_runtime_boundary_contract_id": (
            "objc3c.metaprogramming.expansion.host.runtime.boundary.v1"
        ),
        "lowering_host_cache_surface_model": (
            "metaprogramming-lowering-emission-replay-and-host-cache-packets-freeze-one-live-compile-coupled-boundary-before-runnable-expansion-runtime-package-loading-or-public-abi-widening"
        ),
        "semantic_surface_paths": [
            "frontend.pipeline.semantic_surface.objc_metaprogramming_expansion_and_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_synthesized_ast_and_ir_emission",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_module_interface_and_replay_preservation",
            "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_host_process_and_cache_runtime_integration",
        ],
        "runtime_import_surface_paths": [
            "objc_metaprogramming_module_interface_and_replay_preservation",
            "objc_metaprogramming_macro_host_process_and_cache_runtime_integration",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
            "tests/tooling/fixtures/native/synthesized_ast_ir_macro_positive.objc3",
            "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-runnable-macro-execution-claim",
            "no-runtime-package-loader-readiness-claim",
            "no-public-abi-widening",
        ],
        "requires_runtime_import_surface": True,
        "requires_host_cache_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_cross_module_metaprogramming_artifact_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"cross-module-metaprogramming-artifact-preservation"}
    ]
    return {
        "contract_id": (
            RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_id": (
            RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID
        ),
        "runtime_import_surface_artifact": "<emit-prefix>.runtime-import-surface.json",
        "cross_module_link_plan_artifact": "<emit-prefix>.cross-module-runtime-link-plan.json",
        "module_interface_replay_preservation_contract_id": (
            "objc3c.metaprogramming.module.interface.replay.preservation.v1"
        ),
        "macro_host_process_cache_runtime_integration_contract_id": (
            "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        ),
        "surface_model": (
            "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-metaprogramming-replay-facts-and-host-cache-compatibility-beyond-local-object-emission"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/preservation_provider.objc3",
            "tests/tooling/fixtures/native/preservation_consumer.objc3",
        ],
        "requires_runtime_import_surface": True,
        "requires_cross_module_link_plan": True,
        "requires_real_compile_output": True,
    }


def build_runtime_metaprogramming_runtime_abi_cache_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "metaprogramming-executable-lowering",
            "cross-module-metaprogramming-artifact-preservation",
            "metaprogramming-runtime-abi-cache-surface",
        }
    ]
    return {
        "contract_id": RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.metaprogramming-macro-host-cache.json",
            "<emit-prefix>.runtime-import-surface.json",
        ],
        "source_contract_ids": [
            RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ],
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_metaprogramming_runtime_abi_boundary": (
            PRIVATE_METAPROGRAMMING_RUNTIME_ABI_BOUNDARY
        ),
        "expansion_host_boundary_snapshot_symbol": (
            "objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing"
        ),
        "macro_host_process_cache_integration_snapshot_symbol": (
            "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing"
        ),
        "runtime_abi_boundary_model": METAPROGRAMMING_RUNTIME_ABI_BOUNDARY_MODEL,
        "expansion_runtime_model": METAPROGRAMMING_EXPANSION_RUNTIME_MODEL,
        "host_cache_runtime_model": METAPROGRAMMING_HOST_CACHE_RUNTIME_MODEL,
        "fail_closed_model": METAPROGRAMMING_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/preservation_provider.objc3",
            "tests/tooling/fixtures/native/preservation_consumer.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
            "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_metaprogramming_cache_runtime_integration_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"live-metaprogramming-cache-runtime-integration"}
    ]
    return {
        "contract_id": (
            RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ],
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "macro_host_process_cache_integration_snapshot_symbol": (
            "objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing"
        ),
        "implementation_model": (
            "live-host-cache-artifacts-publish-cache-materialization-truth-and-cross-module-runtime-import-consumers-preserve-the-same-host-cache-runtime-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp"
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
    }


def check_metaprogramming_source_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "metaprogramming-source-surface"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "expansion_lowering_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    semantic_surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_derive_macro_property_behavior_source_closure", {})
    )
    expect(
        isinstance(semantic_surface, dict),
        "expected metaprogramming source fixture to publish objc_metaprogramming_derive_macro_property_behavior_source_closure",
    )
    expected_fields = {
        "contract_id": "objc3c.metaprogramming.metaprogramming.source.closure.v1",
        "frontend_surface_path": (
            "frontend.pipeline.semantic_surface."
            "objc_metaprogramming_derive_macro_property_behavior_source_closure"
        ),
        "source_model": (
            "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-while-expansion-synthesis-and-runtime-behavior-remain-deferred"
        ),
        "failure_model": (
            "metaprogramming-stays-source-closure-only-with-no-macro-expansion-derived-conformance-synthesis-or-property-runtime-claims-yet"
        ),
    }
    for field, expected_value in expected_fields.items():
        expect(
            semantic_surface.get(field) == expected_value,
            f"expected metaprogramming source surface to preserve {field}",
        )
    expect(
        semantic_surface.get("source_only_claim_ids")
        == [
            "source-only:derive-markers",
            "source-only:macro-markers",
            "source-only:property-behavior-markers",
        ],
        "expected metaprogramming source surface to preserve source_only_claim_ids",
    )
    expect(
        semantic_surface.get("derive_marker_sites") == 1,
        "expected metaprogramming source surface to publish one derive marker site",
    )
    expect(
        semantic_surface.get("macro_marker_sites") == 1,
        "expected metaprogramming source surface to publish one macro marker site",
    )
    expect(
        semantic_surface.get("property_behavior_sites") == 2,
        "expected metaprogramming source surface to publish two property behavior sites",
    )
    expect(
        semantic_surface.get("derive_marker_source_supported") is True
        and semantic_surface.get("macro_marker_source_supported") is True
        and semantic_surface.get("property_behavior_source_supported") is True,
        "expected metaprogramming source surface to preserve source support flags",
    )
    expect(
        semantic_surface.get("deterministic_handoff") is True
        and semantic_surface.get("ready_for_semantic_expansion") is True,
        "expected metaprogramming source surface to preserve deterministic source handoff",
    )

    return CaseResult(
        case_id="metaprogramming-source-surface",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": "objc_metaprogramming_derive_macro_property_behavior_source_closure",
            "contract_id": semantic_surface.get("contract_id"),
            "derive_marker_sites": semantic_surface.get("derive_marker_sites"),
            "macro_marker_sites": semantic_surface.get("macro_marker_sites"),
            "property_behavior_sites": semantic_surface.get("property_behavior_sites"),
        },
    )


def check_metaprogramming_package_provenance_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-package-provenance-source-surface"
    fixtures: dict[str, tuple[Path, str, str]] = {
        "macro_package_provenance": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_lowering_positive.objc3",
            "objc_metaprogramming_macro_package_and_provenance_source_completion",
            "objc3c.metaprogramming.macro.package.provenance.source.completion.v1",
        ),
        "property_behavior_source_completion": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_source_completion_positive.objc3",
            "objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion",
            "objc3c.metaprogramming.property.behavior.source.completion.v1",
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        semantic_surface_name,
        expected_contract_id,
    ) in fixtures.items():
        _, _, manifest_path = compile_fixture_outputs(fixture_path, case_dir / fixture_key / "compile")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(semantic_surface_name, {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish {semantic_surface_name}",
        )
        expect(
            semantic_surface.get("contract_id") == expected_contract_id,
            f"expected {fixture_key} fixture to preserve {expected_contract_id}",
        )
        expect(
            semantic_surface.get("deterministic_handoff") is True
            and semantic_surface.get("ready_for_semantic_expansion") is True,
            f"expected {fixture_key} fixture to preserve deterministic source completion handoff",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": semantic_surface_name,
            "contract_id": semantic_surface.get("contract_id"),
        }
        if fixture_key == "macro_package_provenance":
            expect(
                semantic_surface.get("macro_marker_sites") == 1
                and semantic_surface.get("macro_package_sites") == 1
                and semantic_surface.get("macro_provenance_sites") == 1
                and semantic_surface.get("expansion_visible_macro_sites") == 1,
                "expected macro package/provenance source completion to preserve marker and visibility counts",
            )
            expect(
                semantic_surface.get("macro_package_source_supported") is True
                and semantic_surface.get("macro_provenance_source_supported") is True
                and semantic_surface.get("expansion_visible_source_supported") is True,
                "expected macro package/provenance source completion to preserve support flags",
            )
        else:
            expect(
                semantic_surface.get("property_behavior_sites") == 5
                and semantic_surface.get("interface_property_behavior_sites") == 2
                and semantic_surface.get("implementation_property_behavior_sites") == 2
                and semantic_surface.get("protocol_property_behavior_sites") == 1,
                "expected property-behavior source completion to preserve property behavior counts",
            )
            expect(
                semantic_surface.get("synthesized_binding_visible_sites") == 4
                and semantic_surface.get("synthesized_getter_visible_sites") == 5
                and semantic_surface.get("synthesized_setter_visible_sites") == 2,
                "expected property-behavior source completion to preserve synthesized declaration visibility counts",
            )
            expect(
                semantic_surface.get("property_behavior_source_supported") is True
                and semantic_surface.get("synthesized_declaration_visibility_supported")
                is True,
                "expected property-behavior source completion to preserve support flags",
            )

    return CaseResult(
        case_id="metaprogramming-package-provenance-source-surface",
        probe="compile-manifest-source-completion-surface",
        fixture="tests/tooling/fixtures/native/expansion_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )


def check_metaprogramming_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "metaprogramming-semantics"
    fixtures: dict[str, tuple[Path, int, int, int]] = {
        "semantic_model": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_behavior_semantic_model_positive.objc3",
            0,
            1,
            2,
        ),
        "lowering_ready": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "expansion_lowering_positive.objc3",
            1,
            1,
            2,
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        expected_derive_marker_sites,
        expected_macro_marker_sites,
        expected_property_behavior_sites,
    ) in fixtures.items():
        _, _, manifest_path = compile_fixture_outputs(fixture_path, case_dir / fixture_key / "compile")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get("objc_metaprogramming_expansion_and_behavior_semantic_model", {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish objc_metaprogramming_expansion_and_behavior_semantic_model",
        )
        expect(
            semantic_surface.get("contract_id")
            == "objc3c.metaprogramming.expansion.behavior.semantic.model.v1",
            f"expected {fixture_key} fixture to preserve the metaprogramming expansion behavior semantic contract",
        )
        expect(
            semantic_surface.get("frontend_dependency_contract_id")
            == "objc3c.metaprogramming.property.behavior.source.completion.v1",
            f"expected {fixture_key} fixture to preserve the property behavior source dependency contract",
        )
        expect(
            semantic_surface.get("derive_marker_sites") == expected_derive_marker_sites
            and semantic_surface.get("macro_marker_sites") == expected_macro_marker_sites
            and semantic_surface.get("property_behavior_sites")
            == expected_property_behavior_sites,
            f"expected {fixture_key} fixture to preserve semantic site counts",
        )
        expect(
            semantic_surface.get("macro_package_provenance_surface_reused") is True
            and semantic_surface.get("property_behavior_source_supported") is True
            and semantic_surface.get("synthesized_visibility_surface_reused")
            is True,
            f"expected {fixture_key} fixture to preserve semantic surface reuse flags",
        )
        expect(
            semantic_surface.get("derive_synthesis_deferred") is True
            and semantic_surface.get("macro_execution_deferred") is True
            and semantic_surface.get("property_behavior_runtime_deferred") is True,
            f"expected {fixture_key} fixture to preserve deferred runtime semantics",
        )
        expect(
            semantic_surface.get("deterministic") is True
            and semantic_surface.get("ready_for_core_implementation") is True,
            f"expected {fixture_key} fixture to preserve deterministic semantic readiness",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "contract_id": semantic_surface.get("contract_id"),
            "derive_marker_sites": semantic_surface.get("derive_marker_sites"),
            "macro_marker_sites": semantic_surface.get("macro_marker_sites"),
            "property_behavior_sites": semantic_surface.get("property_behavior_sites"),
        }

    return CaseResult(
        case_id="metaprogramming-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/expansion_behavior_semantic_model_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )


def check_metaprogramming_derive_property_behavior_semantics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-derive-property-behavior-semantics"
    derive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "derive_expansion_inventory_positive.objc3"
    )
    property_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_behavior_legality_positive.objc3"
    )
    _, _, derive_manifest_path = compile_fixture_outputs(
        derive_fixture, case_dir / "derive-positive" / "compile"
    )
    _, _, property_manifest_path = compile_fixture_outputs(
        property_fixture, case_dir / "property-positive" / "compile"
    )
    derive_manifest = json.loads(derive_manifest_path.read_text(encoding="utf-8"))
    property_manifest = json.loads(property_manifest_path.read_text(encoding="utf-8"))
    derive_surface = (
        derive_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_derive_expansion_inventory", {})
    )
    property_surface = (
        property_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get(
            "objc_metaprogramming_property_behavior_legality_and_interaction_completion",
            {},
        )
    )
    expect(
        derive_surface.get("contract_id")
        == "objc3c.metaprogramming.derive.expansion.inventory.v1",
        "expected derive expansion inventory fixture to preserve the derive inventory contract",
    )
    expect(
        derive_surface.get("derive_request_sites") == 4
        and derive_surface.get("supported_derive_request_sites") == 4
        and derive_surface.get("generated_method_entry_count") == 4,
        "expected derive expansion inventory fixture to preserve supported derive counts",
    )
    expect(
        derive_surface.get("equatable_alias_sites") == 1
        and derive_surface.get("equality_derive_sites") == 2
        and derive_surface.get("hash_derive_sites") == 1
        and derive_surface.get("debug_description_derive_sites") == 1,
        "expected derive expansion inventory fixture to preserve derive family counts",
    )
    expect(
        derive_surface.get("unsupported_derive_fail_closed") is True
        and derive_surface.get("selector_conflicts_fail_closed") is True
        and derive_surface.get("deterministic") is True
        and derive_surface.get("ready_for_lowering_and_runtime") is True,
        "expected derive expansion inventory fixture to preserve fail-closed readiness",
    )
    expect(
        property_surface.get("contract_id")
        == "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1",
        "expected property behavior legality fixture to preserve the legality contract",
    )
    expect(
        property_surface.get("property_behavior_sites") == 5
        and property_surface.get("supported_behavior_sites") == 5
        and property_surface.get("unsupported_behavior_sites") == 0,
        "expected property behavior legality fixture to preserve behavior counts",
    )
    expect(
        property_surface.get("observed_behavior_sites") == 2
        and property_surface.get("projected_behavior_sites") == 3,
        "expected property behavior legality fixture to preserve observed/projected counts",
    )
    expect(
        property_surface.get("unsupported_behavior_fail_closed") is True
        and property_surface.get("owner_topology_fail_closed") is True
        and property_surface.get("interaction_legality_fail_closed") is True
        and property_surface.get("storage_legality_fail_closed") is True
        and property_surface.get("deterministic") is True
        and property_surface.get("ready_for_lowering_and_runtime") is True,
        "expected property behavior legality fixture to preserve fail-closed readiness",
    )

    negative_fixtures = {
        "unsupported_derive": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "derive_expansion_inventory_negative_unsupported.objc3",
            "O3S317",
            "unsupported derive 'Networked'",
        ),
        "unsupported_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_unsupported.objc3",
            "O3S326",
            "unsupported property behavior 'Cached'",
        ),
        "nonobject_behavior": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_nonobject.objc3",
            "O3S327",
            "requires an Objective-C object property",
        ),
        "protocol_observed": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_protocol_observed.objc3",
            "O3S328",
            "requires a concrete interface or implementation property",
        ),
        "projected_writable": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_behavior_legality_negative_projected_writable.objc3",
            "O3S330",
            "requires a readonly getter-only property",
        ),
    }
    negative_summary: dict[str, Any] = {}
    for negative_key, (fixture_path, expected_code, expected_message) in negative_fixtures.items():
        compile_dir = case_dir / negative_key / "compile"
        compile_result, _ = run_fixture_compile(
            fixture_path,
            compile_dir,
            write_provenance=False,
        )
        expect(
            compile_result.returncode != 0,
            f"expected negative metaprogramming fixture {negative_key} to fail compilation",
        )
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        expect(diagnostics_path.is_file(), f"expected diagnostics for negative fixture {negative_key}")
        diagnostics_text = diagnostics_path.read_text(encoding="utf-8")
        expect(
            expected_code in diagnostics_text and expected_message in diagnostics_text,
            f"expected negative metaprogramming fixture {negative_key} to preserve {expected_code}",
        )
        negative_summary[negative_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "diagnostics": str(diagnostics_path.relative_to(ROOT)).replace("\\", "/"),
            "expected_code": expected_code,
        }

    return CaseResult(
        case_id="metaprogramming-derive-property-behavior-semantics",
        probe="compile-manifest-derive-property-behavior-semantics",
        fixture="tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "derive_positive": {
                "fixture": str(derive_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(derive_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "generated_method_entry_count": derive_surface.get(
                    "generated_method_entry_count"
                ),
                "expansion_inventory_rows_lexicographic": derive_surface.get(
                    "expansion_inventory_rows_lexicographic"
                ),
            },
            "property_positive": {
                "fixture": str(property_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(property_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "supported_behavior_sites": property_surface.get(
                    "supported_behavior_sites"
                ),
                "observed_behavior_sites": property_surface.get(
                    "observed_behavior_sites"
                ),
                "projected_behavior_sites": property_surface.get(
                    "projected_behavior_sites"
                ),
            },
            "negative_cases": negative_summary,
        },
    )


def check_metaprogramming_macro_safety_cache_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-macro-safety-cache-diagnostics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_provider.objc3"
    )
    _, _, positive_manifest_path = compile_fixture_outputs(
        positive_fixture, case_dir / "positive" / "compile"
    )
    positive_manifest = json.loads(positive_manifest_path.read_text(encoding="utf-8"))
    macro_safety_surface = (
        positive_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics", {})
    )
    expect(
        macro_safety_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1",
        "expected macro host process provider fixture to preserve the macro safety semantic contract",
    )
    expect(
        macro_safety_surface.get("macro_marker_sites") == 1
        and macro_safety_surface.get("macro_package_sites") == 1
        and macro_safety_surface.get("macro_provenance_sites") == 1
        and macro_safety_surface.get("expansion_visible_macro_sites") == 1,
        "expected macro host process provider fixture to preserve macro metadata counts",
    )
    expect(
        macro_safety_surface.get("safe_macro_callable_sites") == 1
        and macro_safety_surface.get("incomplete_macro_metadata_sites") == 0
        and macro_safety_surface.get("orphan_macro_metadata_sites") == 0
        and macro_safety_surface.get("invalid_package_sites") == 0
        and macro_safety_surface.get("invalid_provenance_sites") == 0
        and macro_safety_surface.get("nondeterministic_callable_sites") == 0
        and macro_safety_surface.get("unsupported_callable_topology_sites") == 0,
        "expected macro host process provider fixture to preserve fail-closed macro safety counts",
    )
    expect(
        macro_safety_surface.get("metadata_completeness_enforced") is True
        and macro_safety_surface.get("sandbox_namespace_enforced") is True
        and macro_safety_surface.get("provenance_determinism_enforced") is True
        and macro_safety_surface.get("callable_determinism_enforced") is True
        and macro_safety_surface.get("deterministic") is True
        and macro_safety_surface.get("ready_for_lowering_and_runtime") is True,
        "expected macro host process provider fixture to preserve deterministic fail-closed enforcement flags",
    )

    host_cache_path = (
        case_dir / "positive" / "compile" / "module.metaprogramming-macro-host-cache.json"
    )
    expect(
        host_cache_path.is_file(),
        "expected macro host process provider fixture to publish module.metaprogramming-macro-host-cache.json",
    )
    host_cache_surface = json.loads(host_cache_path.read_text(encoding="utf-8"))
    expect(
        host_cache_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected macro host process provider fixture to publish the metaprogramming host-cache integration contract",
    )
    expect(
        host_cache_surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected macro host process provider fixture to preserve the host runtime boundary source contract",
    )
    expect(
        host_cache_surface.get("host_executable_relative_path")
        == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
        and host_cache_surface.get("cache_root_relative_path")
        == "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "expected macro host process provider fixture to preserve host executable and cache root compatibility paths",
    )
    expect(
        host_cache_surface.get("deterministic") is True
        and host_cache_surface.get("host_process_exit_code") == 0
        and isinstance(host_cache_surface.get("cache_hit"), bool),
        "expected macro host process provider fixture to preserve deterministic host-cache readiness",
    )

    runtime_import_path = case_dir / "positive" / "compile" / "module.runtime-import-surface.json"
    expect(
        runtime_import_path.is_file(),
        "expected macro host process provider fixture to publish module.runtime-import-surface.json",
    )
    runtime_import_surface = json.loads(runtime_import_path.read_text(encoding="utf-8"))
    host_cache_import_surface = runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect(
        host_cache_import_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected runtime import surface to preserve the metaprogramming host-cache integration contract",
    )
    expect(
        host_cache_import_surface.get("runtime_import_artifact_ready") is True
        and host_cache_import_surface.get("separate_compilation_ready") is True
        and host_cache_import_surface.get("deterministic") is True,
        "expected runtime import surface to preserve host-cache compatibility readiness",
    )

    negative_fixtures = {
        "missing_metadata": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_missing_metadata.objc3",
            "O3S320",
            "requires both objc_macro_package and objc_macro_provenance",
        ),
        "orphan_metadata": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_orphan_metadata.objc3",
            "O3S321",
            "macro package/provenance markers require objc_macro",
        ),
        "invalid_package": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_invalid_package.objc3",
            "O3S322",
            "macro sandbox rejected package 'thirdparty.runtime'",
        ),
        "invalid_provenance": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_invalid_provenance.objc3",
            "O3S323",
            "macro provenance must be a lowercase sha256 digest",
        ),
        "nonpure_callable": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_nonpure.objc3",
            "O3S324",
            "must be pure, body-backed, non-async, and non-throws",
        ),
        "method_topology": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "macro_safety_sandbox_negative_method_topology.objc3",
            "O3S325",
            "not sandbox-admitted",
        ),
    }
    negative_batch = compile_negative_diagnostic_batch(
        case_id="metaprogramming-macro-safety-cache-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key=negative_key,
                fixture=fixture_path,
                expected_snippets=[expected_message],
                expected_codes=[expected_code],
            )
            for negative_key, (fixture_path, expected_code, expected_message) in negative_fixtures.items()
        ],
    )
    negative_summary = {
        entry["key"]: {
            "fixture": entry["fixture"],
            "diagnostics": entry["diagnostics"],
            "expected_code": entry["expected_codes"][0],
            "duration_seconds": entry["duration_seconds"],
        }
        for entry in negative_batch["results"]
    }

    return CaseResult(
        case_id="metaprogramming-macro-safety-cache-diagnostics",
        probe="compile-manifest-macro-safety-cache-diagnostics",
        fixture="tests/tooling/fixtures/native/macro_host_process_provider.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "positive_fixture": {
                "fixture": str(positive_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(positive_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "host_cache_artifact": str(host_cache_path.relative_to(ROOT)).replace("\\", "/"),
                "runtime_import_surface": str(runtime_import_path.relative_to(ROOT)).replace("\\", "/"),
                "safe_macro_callable_sites": macro_safety_surface.get(
                    "safe_macro_callable_sites"
                ),
                "cache_hit": host_cache_surface.get("cache_hit"),
                "host_process_exit_code": host_cache_surface.get(
                    "host_process_exit_code"
                ),
            },
            "negative_cases": negative_summary,
            "negative_diagnostics_batch": negative_batch,
        },
    )


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


def check_metaprogramming_runtime_abi_cache_surface_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "metaprogramming-runtime-abi-cache-surface"

    boundary_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "expansion_host_runtime_boundary_positive.objc3"
    )
    boundary_obj_path, _, boundary_manifest_path = compile_fixture_outputs(
        boundary_fixture, case_dir / "boundary" / "compile"
    )
    boundary_probe = (
        ROOT / "tests" / "tooling" / "runtime" / "expansion_host_runtime_boundary_probe.cpp"
    )
    boundary_exe = case_dir / "boundary" / "expansion_host_runtime_boundary_probe.exe"
    compile_probe(clangxx, boundary_probe, boundary_exe, [boundary_obj_path])
    boundary_payload = parse_key_value_output(
        run_probe(boundary_exe), "metaprogramming runtime ABI expansion boundary probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "property_runtime_ready": 1,
        "macro_host_execution_ready": 0,
        "macro_host_process_launch_ready": 0,
        "runtime_package_loader_ready": 0,
        "deterministic": 1,
        "runtime_support_library_archive_relative_path": "artifacts/lib/objc3_runtime.lib",
        "property_behavior_runtime_model": (
            "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks"
        ),
        "macro_expansion_host_model": (
            "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed"
        ),
        "fail_closed_model": "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet",
    }.items():
        expect(
            boundary_payload.get(field_name) == expected_value,
            f"expected metaprogramming runtime ABI expansion boundary probe to preserve {field_name}",
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
    host_cache_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "macro_host_process_cache_integration_probe.cpp"
    )
    host_cache_exe = case_dir / "host-cache" / "macro_host_process_cache_integration_probe.exe"
    compile_probe(clangxx, host_cache_probe, host_cache_exe, [])
    host_cache_payload = parse_key_value_output(
        run_probe(host_cache_exe), "metaprogramming runtime ABI host-cache probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "property_runtime_ready": 1,
        "macro_host_execution_ready": 1,
        "macro_host_process_launch_ready": 1,
        "runtime_package_loader_ready": 0,
        "deterministic": 1,
        "host_executable_relative_path": "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        "cache_root_relative_path": "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "host_model": (
            "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization"
        ),
        "toolchain_model": (
            "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization"
        ),
        "cache_model": (
            "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs"
        ),
        "fail_closed_model": (
            "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims"
        ),
    }.items():
        expect(
            host_cache_payload.get(field_name) == expected_value,
            f"expected metaprogramming runtime ABI host-cache probe to preserve {field_name}",
        )

    host_cache_artifact_path = (
        case_dir / "host-cache" / "compile" / "module.metaprogramming-macro-host-cache.json"
    )
    runtime_import_path = case_dir / "host-cache" / "compile" / "module.runtime-import-surface.json"
    host_cache_artifact = json.loads(host_cache_artifact_path.read_text(encoding="utf-8"))
    runtime_import_surface = json.loads(runtime_import_path.read_text(encoding="utf-8"))
    host_cache_import_surface = runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    expect(
        host_cache_artifact.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1"
        and host_cache_import_surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected metaprogramming runtime ABI host-cache artifact and runtime import surface to preserve the host-cache integration contract",
    )
    for field_name in (
        "host_executable_relative_path",
        "cache_root_relative_path",
        "deterministic",
        "replay_key",
    ):
        expect(
            host_cache_artifact.get(field_name) == host_cache_import_surface.get(field_name),
            f"expected metaprogramming runtime ABI host-cache artifact and runtime import surface to preserve {field_name}",
        )
    expect(
        host_cache_artifact.get("host_executable_relative_path")
        == host_cache_payload.get("host_executable_relative_path")
        and host_cache_artifact.get("cache_root_relative_path")
        == host_cache_payload.get("cache_root_relative_path"),
        "expected metaprogramming runtime ABI host-cache artifact to stay aligned with the runtime snapshot paths",
    )

    return CaseResult(
        case_id="metaprogramming-runtime-abi-cache-surface",
        probe="tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp;tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        fixture="tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
        claim_class="compile-linked-runtime-probe",
        passed=True,
        summary={
            "boundary_fixture": {
                "fixture": str(boundary_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(boundary_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "probe": str(boundary_probe.relative_to(ROOT)).replace("\\", "/"),
                "probe_exe": str(boundary_exe.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
                "macro_host_execution_ready": boundary_payload.get(
                    "macro_host_execution_ready"
                ),
            },
            "host_cache_fixture": {
                "fixture": str(host_fixture.relative_to(ROOT)).replace("\\", "/"),
                "manifest": str(host_manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "host_cache_artifact": str(host_cache_artifact_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "runtime_import_surface": str(runtime_import_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "probe": str(host_cache_probe.relative_to(ROOT)).replace("\\", "/"),
                "probe_exe": str(host_cache_exe.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": host_cache_artifact.get("contract_id"),
                "host_executable_relative_path": host_cache_payload.get(
                    "host_executable_relative_path"
                ),
                "cache_root_relative_path": host_cache_payload.get(
                    "cache_root_relative_path"
                ),
            },
        },
    )


def check_live_metaprogramming_cache_runtime_integration_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-metaprogramming-cache-runtime-integration"
    case_dir.mkdir(parents=True, exist_ok=True)

    provider_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_provider.objc3"
    )
    provider_source = provider_fixture.read_text(encoding="utf-8")
    unique_suffix = datetime.now().strftime("%H%M%S%f")
    unique_module_name = f"MetaprogrammingHostProcessProvider{unique_suffix}"
    cache_root = case_dir / "metaprogramming-cache-root"
    cache_root_override = repo_display_path(cache_root)
    cache_root_args = [
        "--objc3-metaprogramming-cache-root",
        cache_root_override,
    ]
    provider_source = provider_source.replace(
        "module MetaprogrammingHostProcessProvider;",
        f"module {unique_module_name};",
        1,
    )
    temp_provider_fixture = case_dir / "metaprogramming_cache_provider_materialize.objc3"
    first_compile_dir: Path | None = None
    first_host_cache_artifact_path: Path | None = None
    first_runtime_import_path: Path | None = None
    first_host_cache_artifact: dict[str, Any] | None = None
    first_runtime_import_surface: dict[str, Any] | None = None
    first_host_cache_import_surface: dict[str, Any] | None = None

    def compile_candidate(
        compile_dir: Path,
    ) -> tuple[Path, Path, dict[str, Any], dict[str, Any], dict[str, Any]]:
        compile_fixture_with_args(
            temp_provider_fixture,
            compile_dir,
            [
                "--objc3-bootstrap-registration-order-ordinal",
                "1",
                *cache_root_args,
            ],
        )
        host_cache_artifact_path = (
            compile_dir / "module.metaprogramming-macro-host-cache.json"
        )
        runtime_import_path = compile_dir / "module.runtime-import-surface.json"
        host_cache_artifact = json.loads(
            host_cache_artifact_path.read_text(encoding="utf-8")
        )
        runtime_import_surface = json.loads(
            runtime_import_path.read_text(encoding="utf-8")
        )
        host_cache_import_surface = runtime_import_surface.get(
            "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
        )
        return (
            host_cache_artifact_path,
            runtime_import_path,
            host_cache_artifact,
            runtime_import_surface,
            host_cache_import_surface,
        )

    for materialization_attempt in range(0, 16):
        extra_macros = "".join(
            [
                "\n"
                f"pure fn cacheSeed{materialization_attempt}_{index}() -> i32 "
                '__attribute__((objc_macro(named("Trace")), '
                f'objc_macro_package(named("std.metaprogramming.trace.{materialization_attempt}")), '
                f'objc_macro_provenance(named("sha256:{unique_suffix}{index:02d}")))) {{\n'
                f"  return {17 + index};\n"
                "}\n"
                for index in range(materialization_attempt)
            ]
        )
        temp_provider_fixture.write_text(provider_source + extra_macros, encoding="utf-8")
        compile_dir = case_dir / f"provider-first-{materialization_attempt:02d}"
        (
            candidate_host_cache_artifact_path,
            candidate_runtime_import_path,
            candidate_host_cache_artifact,
            candidate_runtime_import_surface,
            candidate_host_cache_import_surface,
        ) = compile_candidate(compile_dir)
        if (
            candidate_host_cache_artifact.get("launch_attempted") is not True
            and remove_metaprogramming_cache_entry_from_artifact(candidate_host_cache_artifact)
        ):
            compile_dir = case_dir / f"provider-first-{materialization_attempt:02d}-materialize"
            (
                candidate_host_cache_artifact_path,
                candidate_runtime_import_path,
                candidate_host_cache_artifact,
                candidate_runtime_import_surface,
                candidate_host_cache_import_surface,
            ) = compile_candidate(compile_dir)
        if candidate_host_cache_artifact.get("launch_attempted") is True:
            first_compile_dir = compile_dir
            first_host_cache_artifact_path = candidate_host_cache_artifact_path
            first_runtime_import_path = candidate_runtime_import_path
            first_host_cache_artifact = candidate_host_cache_artifact
            first_runtime_import_surface = candidate_runtime_import_surface
            first_host_cache_import_surface = candidate_host_cache_import_surface
            break
    expect(
        first_compile_dir is not None
        and first_host_cache_artifact_path is not None
        and first_runtime_import_path is not None
        and first_host_cache_artifact is not None
        and first_runtime_import_surface is not None
        and first_host_cache_import_surface is not None,
        "expected live metaprogramming host-cache implementation case to force a materializing cache miss before the cache-hit replay check",
    )
    for field_name, expected_value in {
        "cache_ready": True,
        "launch_attempted": True,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "materialized",
        "host_process_exit_code": 0,
        "deterministic": True,
    }.items():
        expect(
            first_host_cache_artifact.get(field_name) == expected_value,
            f"expected first metaprogramming host-cache materialization artifact to preserve {field_name}",
        )
    for relative_field in (
        "cache_entry_relative_path",
        "cache_summary_relative_path",
        "cache_runtime_import_surface_relative_path",
        "cache_manifest_relative_path",
    ):
        relative_value = first_host_cache_artifact.get(relative_field)
        expect(
            isinstance(relative_value, str) and relative_value != "",
            f"expected first metaprogramming host-cache materialization artifact to publish {relative_field}",
        )
        expect(
            (ROOT / Path(relative_value)).is_file()
            or (ROOT / Path(relative_value)).is_dir(),
            f"expected first metaprogramming host-cache materialization artifact path {relative_field} to exist",
        )
    expect(
        first_runtime_import_surface.get("module_name") == unique_module_name,
        "expected first metaprogramming host-cache materialization compile to publish the unique module name",
    )
    expect(
        first_host_cache_artifact.get("cache_root_relative_path") == cache_root_override
        and first_host_cache_import_surface.get("cache_root_relative_path")
        == cache_root_override,
        "expected first metaprogramming host-cache materialization compile to use the test-owned cache root override",
    )
    expect(
        first_host_cache_artifact.get("cache_root_relative_path")
        != "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "expected live metaprogramming host-cache test not to depend on the shared scratch cache root",
    )
    expect(
        first_host_cache_import_surface.get("host_executable_relative_path")
        == first_host_cache_artifact.get("host_executable_relative_path")
        and first_host_cache_import_surface.get("cache_root_relative_path")
        == first_host_cache_artifact.get("cache_root_relative_path"),
        "expected first metaprogramming host-cache materialization compile to align import-surface and artifact cache paths",
    )

    second_compile_dir = case_dir / "provider-second"
    compile_fixture_with_args(
        temp_provider_fixture,
        second_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "1",
            *cache_root_args,
        ],
    )
    second_host_cache_artifact_path = (
        second_compile_dir / "module.metaprogramming-macro-host-cache.json"
    )
    second_runtime_import_path = second_compile_dir / "module.runtime-import-surface.json"
    second_host_cache_artifact = json.loads(
        second_host_cache_artifact_path.read_text(encoding="utf-8")
    )
    second_runtime_import_surface = json.loads(
        second_runtime_import_path.read_text(encoding="utf-8")
    )
    second_host_cache_import_surface = second_runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    for field_name, expected_value in {
        "cache_ready": True,
        "launch_attempted": False,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "cache-hit",
        "host_process_exit_code": 0,
        "deterministic": True,
    }.items():
        expect(
            second_host_cache_artifact.get(field_name) == expected_value,
            f"expected second metaprogramming host-cache materialization artifact to preserve {field_name}",
        )
    for field_name in (
        "cache_key",
        "cache_entry_relative_path",
        "cache_summary_relative_path",
        "cache_runtime_import_surface_relative_path",
        "cache_manifest_relative_path",
        "host_executable_relative_path",
        "cache_root_relative_path",
        "replay_key",
    ):
        expect(
            second_host_cache_artifact.get(field_name)
            == first_host_cache_artifact.get(field_name),
            f"expected second metaprogramming host-cache materialization artifact to preserve {field_name}",
        )
    expect(
        second_host_cache_import_surface.get("replay_key")
        == first_host_cache_import_surface.get("replay_key"),
        "expected repeated metaprogramming host-cache materialization compile to preserve the same import-surface replay key",
    )

    consumer_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_consumer.objc3"
    )
    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(first_runtime_import_path),
            *cache_root_args,
        ],
    )
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
            first_host_cache_artifact.get("host_executable_relative_path"),
        ),
        (
            "expected_metaprogramming_host_cache_root_relative_path",
            first_host_cache_artifact.get("cache_root_relative_path"),
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected live metaprogramming host-cache consumer link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [unique_module_name]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "expected live metaprogramming host-cache consumer link plan to preserve imported module readiness",
    )
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected live metaprogramming host-cache consumer link plan to publish one imported module",
    )
    imported_module = imported_modules[0]
    for field_name, expected_value in (
        ("module_name", unique_module_name),
        ("metaprogramming_macro_host_process_cache_runtime_integration_present", True),
        ("metaprogramming_macro_host_process_cache_runtime_ready", True),
        ("metaprogramming_macro_host_process_cache_separate_compilation_ready", True),
        ("metaprogramming_macro_host_process_cache_deterministic", True),
        (
            "metaprogramming_macro_host_process_cache_host_executable_relative_path",
            first_host_cache_artifact.get("host_executable_relative_path"),
        ),
        (
            "metaprogramming_macro_host_process_cache_root_relative_path",
            first_host_cache_artifact.get("cache_root_relative_path"),
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected live metaprogramming host-cache imported module to preserve {field_name}",
        )

    host_cache_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "macro_host_process_cache_integration_probe.cpp"
    )
    host_cache_exe = case_dir / "macro_host_process_cache_integration_probe.exe"
    compile_probe(clangxx, host_cache_probe, host_cache_exe, [])
    host_cache_payload = parse_key_value_output(
        run_probe(
            host_cache_exe,
            env={"OBJC3C_METAPROGRAMMING_CACHE_ROOT": cache_root_override},
        ),
        "live metaprogramming host-cache runtime integration probe",
    )
    expect(
        host_cache_payload.get("host_executable_relative_path")
        == first_host_cache_artifact.get("host_executable_relative_path")
        and host_cache_payload.get("cache_root_relative_path")
        == first_host_cache_artifact.get("cache_root_relative_path")
        and host_cache_payload.get("macro_host_execution_ready") == 1
        and host_cache_payload.get("macro_host_process_launch_ready") == 1,
        "expected live metaprogramming host-cache runtime probe to stay aligned with the cache artifact paths and readiness",
    )

    return CaseResult(
        case_id="live-metaprogramming-cache-runtime-integration",
        probe="tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        fixture=str(temp_provider_fixture.relative_to(ROOT)).replace("\\", "/"),
        claim_class="compile-linked-runtime-probe",
        passed=True,
        summary={
            "provider_fixture": str(temp_provider_fixture.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "provider_module_name": unique_module_name,
            "cache_root_override_flag": "--objc3-metaprogramming-cache-root",
            "cache_root_environment_variable": "OBJC3C_METAPROGRAMMING_CACHE_ROOT",
            "cache_root_relative_path": cache_root_override,
            "cache_root_is_test_owned": True,
            "shared_cache_prune_used": False,
            "first_host_cache_artifact": {
                "path": str(first_host_cache_artifact_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "cache_key": first_host_cache_artifact.get("cache_key"),
                "cache_materialization_state": first_host_cache_artifact.get(
                    "cache_materialization_state"
                ),
                "launch_attempted": first_host_cache_artifact.get("launch_attempted"),
            },
            "second_host_cache_artifact": {
                "path": str(second_host_cache_artifact_path.relative_to(ROOT)).replace(
                    "\\", "/"
                ),
                "cache_key": second_host_cache_artifact.get("cache_key"),
                "cache_materialization_state": second_host_cache_artifact.get(
                    "cache_materialization_state"
                ),
                "launch_attempted": second_host_cache_artifact.get("launch_attempted"),
            },
            "consumer_link_plan": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "imported_module_names": link_plan.get(
                "metaprogramming_host_cache_imported_module_names_lexicographic"
            ),
        },
    )




__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
