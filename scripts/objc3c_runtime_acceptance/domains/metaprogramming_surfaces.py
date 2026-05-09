"""Metaprogramming runtime acceptance contract surfaces."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..core import (
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)

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
