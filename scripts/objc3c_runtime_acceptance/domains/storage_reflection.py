"""Storage and reflection runtime acceptance domain."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import NegativeDiagnosticExpectation
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.native_build import compile_fixture_expect_failure
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.native_build import compile_fixture_with_args
from objc3c_runtime_acceptance.native_build import compile_negative_diagnostic_batch
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
    EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID,
    EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
    EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_LOWERING_SURFACE_CONTRACT_ID,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    ROOT,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
    STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
)

def build_runtime_property_ivar_storage_accessor_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "accessor-storage-lowering-metadata-surface",
            "property-ivar-ordering-semantics",
            "property-reflection-accessor-compatibility-diagnostics",
            "property-synthesis-storage-binding-semantics",
            "storage-legality-semantics",
            "synthesized-accessor-codegen",
            "synthesized-accessor-runtime",
            "property-layout",
            "instance-allocation-layout-runtime",
            "property-execution",
            "property-reflection",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.executable.property.ivar.source.closure.v1",
            "objc3c.executable.property.ivar.source.model.completion.v1",
            "objc3c.executable.property.ivar.semantics.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3PropertyDecl.ivar_binding_symbol",
            "Objc3PropertyDecl.executable_synthesized_binding_kind",
            "Objc3PropertyDecl.executable_synthesized_binding_symbol",
            "Objc3PropertyDecl.property_attribute_profile",
            "Objc3PropertyDecl.effective_getter_selector",
            "Objc3PropertyDecl.effective_setter_available",
            "Objc3PropertyDecl.effective_setter_selector",
            "Objc3PropertyDecl.accessor_ownership_profile",
            "Objc3PropertyDecl.executable_ivar_layout_symbol",
            "Objc3PropertyDecl.executable_ivar_layout_slot_index",
            "Objc3PropertyDecl.executable_ivar_layout_size_bytes",
            "Objc3PropertyDecl.executable_ivar_layout_alignment_bytes",
            "Objc3PropertyDecl.executable_ivar_init_order_index",
            "Objc3PropertyDecl.executable_ivar_destroy_order_index",
            "Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors",
            "Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol",
            "Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol",
            "Objc3ExecutableMetadataPropertyGraphNode.synthesizes_executable_accessors",
            "Objc3ExecutableMetadataPropertyGraphNode.getter_storage_runtime_helper_symbol",
            "Objc3ExecutableMetadataPropertyGraphNode.setter_storage_runtime_helper_symbol",
        ],
        "semantic_boundary_model": (
            "property-ivar-storage-accessor-source-surface-freezes-ast-sema-ir-pipeline-and-runtime-codepaths-before-lowering-or-runtime-semantic-expansion"
        ),
        "source_models": [
            "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization",
            "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles",
            "non-category-class-interface-properties-own-authoritative-default-ivar-and-synthesized-binding-identities-across-implementation-redeclaration-boundaries",
            "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration",
            "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission",
            "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding",
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land",
            "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
            "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols",
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-lowering-owned-storage-or-accessor-semantics-invention",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }










def build_runtime_property_atomicity_synthesis_reflection_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-reflection-accessor-compatibility-diagnostics",
            "storage-legality-semantics",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_sema_pass_manager.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3PropertyDecl.is_atomic",
            "Objc3PropertyDecl.is_nonatomic",
            "Objc3PropertyDecl.has_atomicity_conflict",
            "Objc3PropertyDecl.property_attribute_profile",
            "objc3_runtime_property_entry_snapshot.property_attribute_profile",
        ],
        "source_surface_model": (
            "property-atomicity-synthesis-reflection-source-surface-freezes-atomicity-flags-conflict-state-attribute-profiles-and-private-reflection-codepaths-before-runtime-managed-atomic-storage-semantics-land"
        ),
        "atomicity_fail_closed_model": (
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"
        ),
        "reflection_boundary_model": (
            "property-attribute-profiles-remain-the-authoritative-reflection-carrier-for-atomicity-and-synthesis-state-on-the-private-property-query-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/property_atomic_ownership_negative.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-atomic-property-runtime-abi-widening",
            "no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_dispatch_and_synthesized_accessor_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "accessor-storage-lowering-metadata-surface",
            "property-synthesis-storage-binding-semantics",
            "synthesized-accessor-codegen",
            "synthesized-accessor-runtime",
            "property-layout",
            "instance-allocation-layout-runtime",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.executable.property.accessor.layout.lowering.v1",
            "objc3c.executable.ivar.layout.emission.v1",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "lowering_metadata_model": (
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path"
        ),
        "helper_selection_model": (
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-lowering-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_executable_property_accessor_layout_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-accessor-layout-lowering",
            "property-synthesis-storage-binding-semantics",
            "property-layout",
            "instance-allocation-layout-runtime",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_executable_ivar_layout_emission_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-accessor-layout-lowering",
            "property-ivar-ordering-semantics",
            "property-layout",
            "instance-allocation-layout-runtime",
            "property-execution",
        }
    ]
    return {
        "contract_id": EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering"
        ),
        "offset_global_model": (
            "one-retained-i64-offset-global-per-emitted-ivar-binding"
        ),
        "layout_table_model": (
            "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-runtime-layout-rederivation",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_executable_synthesized_accessor_property_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-accessor-layout-lowering",
            "property-synthesis-storage-binding-semantics",
            "synthesized-accessor-codegen",
            "synthesized-accessor-runtime",
            "property-layout",
            "instance-allocation-layout-runtime",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": (
            EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_LOWERING_SURFACE_CONTRACT_ID
        ),
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
            DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "source_model": (
            "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists"
        ),
        "storage_model": (
            "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals"
        ),
        "property_descriptor_model": (
            "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-storage-global-fallbacks-or-sidecar-body-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def check_cross_module_storage_reflection_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-storage-reflection-artifact-preservation"
    provider_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    provider_storage_surface = provider_import_payload.get(
        "objc_runtime_storage_reflection_artifact_preservation", {}
    )
    expect(
        isinstance(provider_storage_surface, dict),
        "expected storage-reflection provider import surface to publish the preservation packet",
    )
    expected_provider_storage_fields = {
        "contract_id": RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        "source_contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        "executable_property_accessor_layout_lowering_contract_id": "objc3c.executable.property.accessor.layout.lowering.v1",
        "executable_ivar_layout_emission_contract_id": "objc3c.executable.ivar.layout.emission.v1",
        "executable_synthesized_accessor_property_lowering_contract_id": "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_storage_reflection_artifact_preservation",
        "import_artifact_member_name": "objc_runtime_storage_reflection_artifact_preservation",
        "source_model": "runtime-metadata-source-records-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-for-separate-compilation",
        "preservation_model": "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission",
        "fail_closed_model": "missing-or-drifted-storage-reflection-preservation-packets-disable-cross-module-storage-reflection-claims",
    }
    for field_name, expected_value in expected_provider_storage_fields.items():
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )
    expect(
        provider_storage_surface.get("local_property_descriptor_count")
        == provider_registration_manifest.get("property_descriptor_count")
        == 6,
        "expected storage-reflection provider import surface to preserve six property descriptors",
    )
    expect(
        provider_storage_surface.get("local_ivar_descriptor_count")
        == provider_registration_manifest.get("ivar_descriptor_count")
        == 3,
        "expected storage-reflection provider import surface to preserve three ivar descriptors",
    )
    for field_name, expected_value in (
        ("implementation_owned_property_entries", 3),
        ("synthesized_accessor_owner_entries", 3),
        ("synthesized_getter_entries", 3),
        ("synthesized_setter_entries", 3),
        ("synthesized_accessor_entries", 6),
        ("current_property_read_entries", 3),
        ("current_property_write_entries", 2),
        ("current_property_exchange_entries", 1),
        ("weak_current_property_load_entries", 0),
        ("weak_current_property_store_entries", 0),
        ("ivar_layout_entries", 3),
        ("ivar_layout_owner_entries", 1),
    ):
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )
    expect(
        provider_storage_surface.get("runtime_import_artifact_ready") is True
        and provider_storage_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_storage_surface.get("deterministic") is True,
        "expected storage-reflection provider import surface to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        isinstance(provider_storage_surface.get("replay_key"), str)
        and provider_storage_surface.get("replay_key") != "",
        "expected storage-reflection provider import surface to publish a replay key",
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
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    for field_name, expected_value in (
        (
            "runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id",
            RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "runtime_property_ivar_storage_accessor_source_surface_contract_id",
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        ),
        (
            "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        ),
        (
            "executable_property_accessor_layout_lowering_contract_id",
            "objc3c.executable.property.accessor.layout.lowering.v1",
        ),
        (
            "executable_ivar_layout_emission_contract_id",
            "objc3c.executable.ivar.layout.emission.v1",
        ),
        (
            "executable_synthesized_accessor_property_lowering_contract_id",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
        ),
        (
            "storage_reflection_artifact_preservation_model",
            "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission",
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("storage_reflection_cross_module_preservation_ready") is True,
        "expected cross-module link plan to mark storage/reflection preservation ready",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected storage-reflection link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == provider_import_payload.get("module_name")
        == "synthesizedAccessorPropertyLowering",
        "expected storage-reflection link plan to preserve the provider module name",
    )
    for field_name, expected_value in (
        ("storage_reflection_artifact_preservation_present", True),
        ("storage_reflection_runtime_import_artifact_ready", True),
        ("storage_reflection_separate_compilation_preservation_ready", True),
        ("storage_reflection_deterministic", True),
        (
            "storage_reflection_contract_id",
            RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ),
        (
            "storage_reflection_source_contract_id",
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        ),
        (
            "storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id",
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        ),
        (
            "storage_reflection_executable_property_accessor_layout_lowering_contract_id",
            "objc3c.executable.property.accessor.layout.lowering.v1",
        ),
        (
            "storage_reflection_executable_ivar_layout_emission_contract_id",
            "objc3c.executable.ivar.layout.emission.v1",
        ),
        (
            "storage_reflection_executable_synthesized_accessor_property_lowering_contract_id",
            "objc3c.executable.synthesized.accessor.property.lowering.v1",
        ),
        ("storage_reflection_local_property_descriptor_count", 6),
        ("storage_reflection_local_ivar_descriptor_count", 3),
        ("storage_reflection_implementation_owned_property_entries", 3),
        ("storage_reflection_synthesized_accessor_owner_entries", 3),
        ("storage_reflection_synthesized_getter_entries", 3),
        ("storage_reflection_synthesized_setter_entries", 3),
        ("storage_reflection_synthesized_accessor_entries", 6),
        ("storage_reflection_current_property_read_entries", 3),
        ("storage_reflection_current_property_write_entries", 2),
        ("storage_reflection_current_property_exchange_entries", 1),
        ("storage_reflection_weak_current_property_load_entries", 0),
        ("storage_reflection_weak_current_property_store_entries", 0),
        ("storage_reflection_ivar_layout_entries", 3),
        ("storage_reflection_ivar_layout_owner_entries", 1),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported storage-reflection module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("storage_reflection_replay_key"), str)
        and imported_module.get("storage_reflection_replay_key") != "",
        "expected imported storage-reflection module to preserve a replay key",
    )

    local_expected = {
        "local_storage_reflection_implementation_owned_property_entries": 0,
        "local_storage_reflection_synthesized_accessor_owner_entries": 0,
        "local_storage_reflection_synthesized_getter_entries": 0,
        "local_storage_reflection_synthesized_setter_entries": 0,
        "local_storage_reflection_synthesized_accessor_entries": 0,
        "local_storage_reflection_current_property_read_entries": 0,
        "local_storage_reflection_current_property_write_entries": 0,
        "local_storage_reflection_current_property_exchange_entries": 0,
        "local_storage_reflection_weak_current_property_load_entries": 0,
        "local_storage_reflection_weak_current_property_store_entries": 0,
        "local_storage_reflection_ivar_layout_entries": 0,
        "local_storage_reflection_ivar_layout_owner_entries": 0,
    }
    imported_expected = {
        "imported_storage_reflection_implementation_owned_property_entries": 3,
        "imported_storage_reflection_synthesized_accessor_owner_entries": 3,
        "imported_storage_reflection_synthesized_getter_entries": 3,
        "imported_storage_reflection_synthesized_setter_entries": 3,
        "imported_storage_reflection_synthesized_accessor_entries": 6,
        "imported_storage_reflection_current_property_read_entries": 3,
        "imported_storage_reflection_current_property_write_entries": 2,
        "imported_storage_reflection_current_property_exchange_entries": 1,
        "imported_storage_reflection_weak_current_property_load_entries": 0,
        "imported_storage_reflection_weak_current_property_store_entries": 0,
        "imported_storage_reflection_ivar_layout_entries": 3,
        "imported_storage_reflection_ivar_layout_owner_entries": 1,
    }
    transitive_expected = {
        "transitive_storage_reflection_implementation_owned_property_entries": 3,
        "transitive_storage_reflection_synthesized_accessor_owner_entries": 3,
        "transitive_storage_reflection_synthesized_getter_entries": 3,
        "transitive_storage_reflection_synthesized_setter_entries": 3,
        "transitive_storage_reflection_synthesized_accessor_entries": 6,
        "transitive_storage_reflection_current_property_read_entries": 3,
        "transitive_storage_reflection_current_property_write_entries": 2,
        "transitive_storage_reflection_current_property_exchange_entries": 1,
        "transitive_storage_reflection_weak_current_property_load_entries": 0,
        "transitive_storage_reflection_weak_current_property_store_entries": 0,
        "transitive_storage_reflection_ivar_layout_entries": 3,
        "transitive_storage_reflection_ivar_layout_owner_entries": 1,
    }
    for field_name, expected_value in local_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in imported_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in transitive_expected.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-storage-reflection-artifact-preservation",
        probe=None,
        fixture=STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": link_plan.get("local_module", {}).get("module_name"),
            "imported_property_descriptor_count": link_plan.get("imported_property_descriptor_count"),
            "imported_ivar_descriptor_count": link_plan.get("imported_ivar_descriptor_count"),
            "imported_synthesized_accessor_entries": link_plan.get(
                "imported_storage_reflection_synthesized_accessor_entries"
            ),
            "imported_ivar_layout_entries": link_plan.get(
                "imported_storage_reflection_ivar_layout_entries"
            ),
        },
    )


def check_property_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-reflection"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "property_metadata_reflection_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_property_metadata_reflection_probe.cpp"
    exe_path = case_dir / "runtime_property_metadata_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property reflection probe")

    widget_entry = payload.get("widget_entry", {})
    token_property = payload.get("token_property", {})
    value_property = payload.get("value_property", {})
    count_property = payload.get("count_property", {})
    missing_property = payload.get("missing_property", {})
    missing_class_property = payload.get("missing_class_property", {})
    registry_after_count = payload.get("registry_state_after_count", {})

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be present")
    expect(token_property.get("found") == 1, "expected token property to be reflectable")
    expect(token_property.get("setter_available") == 0, "expected readonly token property to have no setter")
    expect(token_property.get("has_runtime_getter") == 1, "expected token property getter to be runtime-backed")
    expect(value_property.get("found") == 1, "expected value property to be reflectable")
    expect(value_property.get("setter_available") == 1, "expected value property to expose a setter")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property getter/setter to be runtime-backed")
    expect(count_property.get("found") == 1, "expected count property to be reflectable")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property getter/setter to be runtime-backed")
    expect(registry_after_count.get("slot_backed_property_count", 0) >= 3,
           "expected slot-backed property registry to include the three Widget properties")
    expect(missing_property.get("found") == 0, "expected missing property lookup to fail closed")
    expect(missing_class_property.get("found") == 0, "expected missing class property lookup to fail closed")

    return CaseResult(
        case_id="property-reflection",
        probe="tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "reflectable_property_count": registry_after_count.get("reflectable_property_count"),
            "slot_backed_property_count": registry_after_count.get("slot_backed_property_count"),
            "value_property_setter_available": value_property.get("setter_available"),
            "count_property_runtime_setter": count_property.get("has_runtime_setter"),
        },
    )


def check_property_execution_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-execution"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "property_ivar_execution_matrix_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "property_ivar_execution_matrix_probe.cpp"
    exe_path = case_dir / "property_ivar_execution_matrix_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property execution probe")

    widget_entry = payload.get("widget_entry", {})
    registry_state = payload.get("registry_state", {})
    count_property = payload.get("count_property", {})
    enabled_property = payload.get("enabled_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})
    count_method = payload.get("count_method", {})
    enabled_method = payload.get("enabled_method", {})
    value_method = payload.get("value_method", {})
    token_method = payload.get("token_method", {})
    set_count_dispatch = payload.get("set_count_dispatch", {})
    count_dispatch = payload.get("count_dispatch", {})
    set_enabled_dispatch = payload.get("set_enabled_dispatch", {})
    enabled_dispatch = payload.get("enabled_dispatch", {})
    set_value_dispatch = payload.get("set_value_dispatch", {})
    value_dispatch = payload.get("value_dispatch", {})
    token_dispatch = payload.get("token_dispatch", {})

    expect(payload.get("widget_instance", 0) != 0, "expected alloc to materialize a Widget instance")
    expect(payload.get("count_value") == 37, "expected synthesized count getter to return the stored value")
    expect(payload.get("enabled_value") == 1, "expected synthesized enabled getter to return the stored value")
    expect(payload.get("value_result") == 55, "expected synthesized strong property getter to return the stored value")
    expect(widget_entry.get("found") == 1, "expected Widget to be realized during property execution")
    expect(widget_entry.get("runtime_property_accessor_count", 0) >= 4,
           "expected Widget to publish runtime-backed synthesized accessors")
    expect(registry_state.get("slot_backed_property_count", 0) >= 4,
           "expected property execution fixture to register four slot-backed properties")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property to execute through runtime-backed synthesized accessors")
    expect(enabled_property.get("has_runtime_getter") == 1 and enabled_property.get("has_runtime_setter") == 1,
           "expected enabled property to execute through runtime-backed synthesized accessors")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property to execute through runtime-backed synthesized accessors")
    expect(token_property.get("has_runtime_getter") == 1 and token_property.get("setter_available") == 0,
           "expected readonly token property to expose only the synthesized getter")
    expect(count_property.get("property_name") == "count",
           "expected count property reflection to stay coherent")
    expect(count_property.get("effective_getter_selector") == "count",
           "expected count getter selector reflection to stay coherent")
    expect(count_property.get("effective_setter_selector") == "setCount:",
           "expected count setter selector reflection to stay coherent")
    expect(enabled_property.get("effective_getter_selector") == "enabled",
           "expected enabled getter selector reflection to stay coherent")
    expect(enabled_property.get("effective_setter_selector") == "setEnabled:",
           "expected enabled setter selector reflection to stay coherent")
    expect(value_property.get("effective_getter_selector") == "currentValue",
           "expected value getter selector reflection to stay coherent")
    expect(value_property.get("effective_setter_selector") == "setCurrentValue:",
           "expected value setter selector reflection to stay coherent")
    expect(token_property.get("effective_getter_selector") == "tokenValue",
           "expected token getter selector reflection to stay coherent")
    expect(count_property.get("getter_owner_identity"), "expected count getter owner identity to be published")
    expect(count_property.get("setter_owner_identity"), "expected count setter owner identity to be published")
    expect(enabled_property.get("getter_owner_identity"), "expected enabled getter owner identity to be published")
    expect(enabled_property.get("setter_owner_identity"), "expected enabled setter owner identity to be published")
    expect(value_property.get("getter_owner_identity"), "expected value getter owner identity to be published")
    expect(value_property.get("setter_owner_identity"), "expected value setter owner identity to be published")
    expect(token_property.get("getter_owner_identity"), "expected token getter owner identity to be published")
    expect(token_property.get("setter_owner_identity") is None,
           "did not expect readonly token property to publish a setter owner identity")
    expect(count_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected count property base identity to match the realized Widget class")
    expect(enabled_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected enabled property base identity to match the realized Widget class")
    expect(value_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected value property base identity to match the realized Widget class")
    expect(token_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected token property base identity to match the realized Widget class")
    expect(registry_state.get("last_resolved_class_name") == "Widget",
           "expected property registry to resolve Widget during live accessor execution")
    expect(registry_state.get("last_resolved_owner_identity"),
           "expected property registry to publish the resolved owner identity")
    expect(count_method.get("resolved") == 1 and count_method.get("parameter_count") == 0,
           "expected count getter dispatch to resolve live through the runtime cache")
    expect(enabled_method.get("resolved") == 1 and enabled_method.get("parameter_count") == 0,
           "expected enabled getter dispatch to resolve live through the runtime cache")
    expect(value_method.get("resolved") == 1 and value_method.get("parameter_count") == 0,
           "expected currentValue getter dispatch to resolve live through the runtime cache")
    expect(token_method.get("resolved") == 1 and token_method.get("parameter_count") == 0,
           "expected tokenValue getter dispatch to resolve live through the runtime cache")
    expect(count_method.get("resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter cache ownership to match reflected property ownership")
    expect(enabled_method.get("resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter cache ownership to match reflected property ownership")
    expect(value_method.get("resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter cache ownership to match reflected property ownership")
    expect(token_method.get("resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter cache ownership to match reflected property ownership")
    expect(set_count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected setCount: to execute through live synthesized accessor resolution")
    expect(set_count_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCount: to execute through the runtime property-setter builtin")
    expect(set_count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected setCount: dispatch property name to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected setCount: dispatch base identity to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected setCount: dispatch slot index to match reflected property metadata")
    expect(set_count_dispatch.get("last_selector") == count_property.get("effective_setter_selector"),
           "expected setCount: dispatch selector to match reflected property metadata")
    expect(set_count_dispatch.get("last_resolved_owner_identity") == count_property.get("setter_owner_identity"),
           "expected setCount: dispatch ownership to match reflected property metadata")
    expect(set_count_dispatch.get("last_used_builtin") == 1 and set_count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected setCount: to remain builtin-backed and runtime-dispatched")
    expect(set_count_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCount: dispatch to report one setter parameter")
    expect(count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected count getter to execute through live synthesized accessor resolution")
    expect(count_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected count getter to execute through the runtime property-getter builtin")
    expect(count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected count getter dispatch property name to match reflected property metadata")
    expect(count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected count getter dispatch base identity to match reflected property metadata")
    expect(count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected count getter dispatch slot index to match reflected property metadata")
    expect(count_dispatch.get("last_selector") == count_property.get("effective_getter_selector"),
           "expected count getter dispatch selector to match reflected property metadata")
    expect(count_dispatch.get("last_resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter dispatch ownership to match reflected property metadata")
    expect(count_dispatch.get("last_used_builtin") == 1 and count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected count getter to remain builtin-backed and runtime-dispatched")
    expect(count_dispatch.get("last_resolved_parameter_count") == 0,
           "expected count getter dispatch to report zero getter parameters")
    expect(set_enabled_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setEnabled: to execute through the runtime property-setter builtin")
    expect(set_enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected setEnabled: dispatch property name to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected setEnabled: dispatch base identity to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected setEnabled: dispatch slot index to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_selector") == enabled_property.get("effective_setter_selector"),
           "expected setEnabled: dispatch selector to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("setter_owner_identity"),
           "expected setEnabled: dispatch ownership to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_used_builtin") == 1 and set_enabled_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setEnabled: to remain builtin-backed and report one setter parameter")
    expect(enabled_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected enabled getter to execute through the runtime property-getter builtin")
    expect(enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected enabled getter dispatch property name to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected enabled getter dispatch base identity to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected enabled getter dispatch slot index to match reflected property metadata")
    expect(enabled_dispatch.get("last_selector") == enabled_property.get("effective_getter_selector"),
           "expected enabled getter dispatch selector to match reflected property metadata")
    expect(enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter dispatch ownership to match reflected property metadata")
    expect(enabled_dispatch.get("last_used_builtin") == 1 and enabled_dispatch.get("last_resolved_parameter_count") == 0,
           "expected enabled getter to remain builtin-backed and report zero getter parameters")
    expect(set_value_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCurrentValue: to execute through the runtime property-setter builtin")
    expect(set_value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected setCurrentValue: dispatch property name to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected setCurrentValue: dispatch base identity to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected setCurrentValue: dispatch slot index to match reflected property metadata")
    expect(set_value_dispatch.get("last_selector") == value_property.get("effective_setter_selector"),
           "expected setCurrentValue: dispatch selector to match reflected property metadata")
    expect(set_value_dispatch.get("last_resolved_owner_identity") == value_property.get("setter_owner_identity"),
           "expected setCurrentValue: dispatch ownership to match reflected property metadata")
    expect(set_value_dispatch.get("last_used_builtin") == 1 and set_value_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCurrentValue: to remain builtin-backed and report one setter parameter")
    expect(value_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected currentValue getter to execute through the runtime property-getter builtin")
    expect(value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected currentValue getter dispatch property name to match reflected property metadata")
    expect(value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected currentValue getter dispatch base identity to match reflected property metadata")
    expect(value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected currentValue getter dispatch slot index to match reflected property metadata")
    expect(value_dispatch.get("last_selector") == value_property.get("effective_getter_selector"),
           "expected currentValue getter dispatch selector to match reflected property metadata")
    expect(value_dispatch.get("last_resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter dispatch ownership to match reflected property metadata")
    expect(value_dispatch.get("last_used_builtin") == 1 and value_dispatch.get("last_resolved_parameter_count") == 0,
           "expected currentValue getter to remain builtin-backed and report zero getter parameters")
    expect(token_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected tokenValue getter to execute through the runtime property-getter builtin")
    expect(token_dispatch.get("last_property_name") == token_property.get("property_name"),
           "expected tokenValue getter dispatch property name to match reflected property metadata")
    expect(token_dispatch.get("last_property_base_identity") == token_property.get("base_identity"),
           "expected tokenValue getter dispatch base identity to match reflected property metadata")
    expect(token_dispatch.get("last_property_slot_index") == token_property.get("slot_index"),
           "expected tokenValue getter dispatch slot index to match reflected property metadata")
    expect(token_dispatch.get("last_selector") == token_property.get("effective_getter_selector"),
           "expected tokenValue getter dispatch selector to match reflected property metadata")
    expect(token_dispatch.get("last_resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter dispatch ownership to match reflected property metadata")
    expect(token_dispatch.get("last_used_builtin") == 1 and token_dispatch.get("last_resolved_parameter_count") == 0,
           "expected tokenValue getter to remain builtin-backed and report zero getter parameters")
    return CaseResult(
        case_id="property-execution",
        probe="tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        fixture="tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "count_value": payload.get("count_value"),
            "enabled_value": payload.get("enabled_value"),
            "value_result": payload.get("value_result"),
            "runtime_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "slot_backed_property_count": registry_state.get("slot_backed_property_count"),
            "count_dispatch_kind": count_dispatch.get("last_implementation_kind"),
            "value_dispatch_kind": value_dispatch.get("last_implementation_kind"),
            "token_dispatch_kind": token_dispatch.get("last_implementation_kind"),
        },
    )











def check_storage_ownership_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-ownership-reflection"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_reflection_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_backed_storage_ownership_reflection_probe.cpp"
    exe_path = case_dir / "runtime_backed_storage_ownership_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "storage ownership reflection probe")

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    box_entry = payload.get("box_entry", {})
    implementation_surface = payload.get("implementation_surface", {})
    manifest_implementation_surface = manifest.get(
        "runtime_property_ivar_accessor_reflection_implementation_surface", {}
    )

    expect(box_entry.get("found") == 1, "expected Box to be realized for storage ownership reflection")
    expect(box_entry.get("runtime_property_accessor_count", 0) >= 5,
           "expected Box to publish five runtime-backed storage accessors")
    expect(box_entry.get("runtime_instance_size_bytes", 0) >= 40,
           "expected Box instance layout to reserve five object-backed storage slots")
    expect(registration_manifest.get("property_descriptor_count") == 10,
           "expected storage ownership fixture to publish ten property descriptors")
    expect(registration_manifest.get("ivar_descriptor_count") == 5,
           "expected storage ownership fixture to publish five ivar layout descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_property_descriptor_count") == 10,
           "expected compile-output truthfulness to certify ten property descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count") == 5,
           "expected compile-output truthfulness to certify five ivar descriptors")
    expect(
        "; runtime_backed_object_ownership_attribute_surface = "
        "contract=objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime-backed object ownership attribute surface",
    )
    expect("property_attribute_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten property-attribute profiles")
    expect("ownership_lifetime_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten ownership lifetime profiles")
    expect("ownership_runtime_hook_profiles=6" in ll_text,
           "expected LLVM IR ownership surface to publish six runtime hook profiles")
    expect("accessor_ownership_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten accessor ownership profiles")
    expected_manifest_implementation_surface = {
        "contract_id": RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "storage_accessor_runtime_abi_surface_contract_id": (
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "implementation_snapshot_symbol": (
            "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "implementation_model": (
            "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation"
        ),
        "reflection_model": (
            "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
        ),
        "fail_closed_model": (
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis"
        ),
    }
    for field, expected_value in expected_manifest_implementation_surface.items():
        expect(
            manifest_implementation_surface.get(field) == expected_value,
            f"expected property/accessor runtime implementation surface to preserve {field}",
        )
    expect(
        manifest_implementation_surface.get("requires_coupled_registration_manifest")
        is True,
        "expected property/accessor runtime implementation surface to require the coupled runtime registration manifest",
    )
    expect(
        manifest_implementation_surface.get("requires_real_compile_output") is True,
        "expected property/accessor runtime implementation surface to require real compile output",
    )
    expect(
        manifest_implementation_surface.get("requires_linked_runtime_probe") is True,
        "expected property/accessor runtime implementation surface to require a linked runtime probe",
    )
    expected_implementation_surface = {
        "property_registry_ready": 1,
        "runtime_accessor_dispatch_ready": 1,
        "runtime_layout_ready": 1,
        "reflection_query_ready": 1,
        "deterministic": 1,
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "implementation_model": (
            "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation"
        ),
        "reflection_model": (
            "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
        ),
        "fail_closed_model": (
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis"
        ),
    }
    for field, expected_value in expected_implementation_surface.items():
        expect(
            implementation_surface.get(field) == expected_value,
            f"expected live storage/accessor implementation snapshot to preserve {field}",
        )

    expected_properties = {
        "current_value_property": {
            "property_name": "currentValue",
            "slot_index": 0,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;strong=1;weak=0;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=nonatomic,strong",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=currentValue;setter_available=1;setter=setCurrentValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "currentValue",
            "effective_setter_selector": "setCurrentValue:",
        },
        "copied_value_property": {
            "property_name": "copiedValue",
            "slot_index": 1,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=1;retain=0;strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=copy,nonatomic",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=copiedValue;setter_available=1;setter=setCopiedValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "copiedValue",
            "effective_setter_selector": "setCopiedValue:",
        },
        "weak_value_property": {
            "property_name": "weakValue",
            "slot_index": 2,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;strong=0;weak=1;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=nonatomic,weak",
            "ownership_lifetime_profile": "weak",
            "ownership_runtime_hook_profile": "objc-weak-side-table",
            "accessor_ownership_profile": "getter=weakValue;setter_available=1;setter=setWeakValue:;ownership_lifetime=weak;runtime_hook=objc-weak-side-table",
            "effective_getter_selector": "weakValue",
            "effective_setter_selector": "setWeakValue:",
        },
        "borrowed_value_property": {
            "property_name": "borrowedValue",
            "slot_index": 3,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=1;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=assign",
            "ownership_lifetime_profile": "unowned-unsafe",
            "ownership_runtime_hook_profile": "objc-unowned-unsafe-direct",
            "accessor_ownership_profile": "getter=borrowedValue;setter_available=1;setter=setBorrowedValue:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct",
            "effective_getter_selector": "borrowedValue",
            "effective_setter_selector": "setBorrowedValue:",
        },
        "guarded_value_property": {
            "property_name": "guardedValue",
            "slot_index": 4,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;strong=0;weak=0;unowned=1;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=unowned",
            "ownership_lifetime_profile": "unowned-safe",
            "ownership_runtime_hook_profile": "objc-unowned-safe-guard",
            "accessor_ownership_profile": "getter=guardedValue;setter_available=1;setter=setGuardedValue:;ownership_lifetime=unowned-safe;runtime_hook=objc-unowned-safe-guard",
            "effective_getter_selector": "guardedValue",
            "effective_setter_selector": "setGuardedValue:",
        },
    }

    for payload_key, expected in expected_properties.items():
        prop = payload.get(payload_key, {})
        expect(prop.get("found") == 1, f"expected {expected['property_name']} to be reflectable")
        expect(prop.get("has_runtime_getter") == 1 and prop.get("has_runtime_setter") == 1,
               f"expected {expected['property_name']} to execute through runtime-backed accessors")
        expect(prop.get("base_identity") == box_entry.get("base_identity"),
               f"expected {expected['property_name']} to share Box base identity")
        expect(prop.get("slot_index") == expected["slot_index"],
               f"expected {expected['property_name']} to keep slot index {expected['slot_index']}")
        expect(prop.get("size_bytes") == 8 and prop.get("alignment_bytes") == 8,
               f"expected {expected['property_name']} to preserve 8-byte object storage layout")
        expect(prop.get("property_name") == expected["property_name"],
               f"expected runtime property name for {expected['property_name']}")
        expect(prop.get("effective_getter_selector") == expected["effective_getter_selector"],
               f"expected getter selector for {expected['property_name']}")
        expect(prop.get("effective_setter_selector") == expected["effective_setter_selector"],
               f"expected setter selector for {expected['property_name']}")
        expect(prop.get("property_attribute_profile") == expected["property_attribute_profile"],
               f"expected property attribute profile for {expected['property_name']}")
        expect(prop.get("ownership_lifetime_profile") == expected["ownership_lifetime_profile"],
               f"expected ownership lifetime profile for {expected['property_name']}")
        expected_runtime_hook = expected["ownership_runtime_hook_profile"]
        if expected_runtime_hook is None:
            expect(prop.get("ownership_runtime_hook_profile") in (None, ""),
                   f"expected no runtime hook profile for {expected['property_name']}")
        else:
            expect(prop.get("ownership_runtime_hook_profile") == expected_runtime_hook,
                   f"expected runtime hook profile for {expected['property_name']}")
        expect(prop.get("accessor_ownership_profile") == expected["accessor_ownership_profile"],
               f"expected accessor ownership profile for {expected['property_name']}")
        expect(prop.get("getter_owner_identity"), f"expected getter owner identity for {expected['property_name']}")
        expect(prop.get("setter_owner_identity"), f"expected setter owner identity for {expected['property_name']}")

    return CaseResult(
        case_id="storage-ownership-reflection",
        probe="tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "runtime_property_accessor_count": box_entry.get("runtime_property_accessor_count"),
            "runtime_instance_size_bytes": box_entry.get("runtime_instance_size_bytes"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "implementation_surface_contract_id": manifest_implementation_surface.get(
                "contract_id"
            ),
            "implementation_snapshot_symbol": manifest_implementation_surface.get(
                "implementation_snapshot_symbol"
            ),
            "guarded_runtime_hook_profile": payload.get("guarded_value_property", {}).get("ownership_runtime_hook_profile"),
            "weak_runtime_hook_profile": payload.get("weak_value_property", {}).get("ownership_runtime_hook_profile"),
        },
    )


def check_storage_legality_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-legality-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_legality_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    sema_pass_manager_manifest = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("sema_pass_manager", {})
    )
    ll_text = ll_path.read_text(encoding="utf-8")

    expect(
        registration_manifest.get("property_descriptor_count") == 10,
        "expected storage legality positive fixture to publish ten property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 5,
        "expected storage legality positive fixture to publish five ivar descriptors",
    )
    expect(
        manifest.get("runtime_property_ivar_storage_accessor_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property/ivar/storage/accessor source surface",
    )
    expect(
        manifest.get(
            "runtime_property_atomicity_synthesis_reflection_source_surface", {}
        ).get("contract_id")
        == RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property atomicity/synthesis/reflection source surface",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_boundary_ready") is True,
        "expected storage legality positive fixture to publish a ready runtime export legality boundary",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_property_attribute_invalid_entries")
        == 0,
        "expected storage legality positive fixture to publish zero invalid property-attribute entries",
    )
    expect(
        sema_pass_manager_manifest.get(
            "runtime_export_property_attribute_contract_violations"
        )
        == 0,
        "expected storage legality positive fixture to publish zero property contract violations",
    )
    expect(
        sema_pass_manager_manifest.get("runtime_export_property_ivar_binding_missing")
        == 0,
        "expected storage legality positive fixture to publish zero missing property ivar bindings",
    )
    expect(
        sema_pass_manager_manifest.get(
            "runtime_export_property_ivar_binding_conflicts"
        )
        == 0,
        "expected storage legality positive fixture to publish zero conflicting property ivar bindings",
    )
    for needle, label in (
        ("runtime_backed_storage_ownership_legality", "runtime-backed storage ownership legality"),
        ("property_attribute_profiles=10", "ten property-attribute profiles"),
        ("accessor_ownership_profiles=10", "ten accessor ownership profiles"),
    ):
        expect(
            needle in ll_text,
            f"expected storage legality positive fixture to publish {label} in LLVM IR",
        )

    negative_batch = compile_negative_diagnostic_batch(
        case_id="storage-legality-semantics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="negative-atomic-ownership",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_atomic_ownership_negative.objc3",
                expected_snippets=[
                    "atomic ownership-aware property 'value' in interface 'Widget' is unsupported until executable accessor storage semantics land"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-weak-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "runtime_backed_storage_ownership_weak_mismatch_negative.objc3",
                expected_snippets=[
                    "property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-unowned-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "runtime_backed_storage_ownership_unowned_mismatch_negative.objc3",
                expected_snippets=[
                    "property ownership qualifier '__unsafe_unretained' conflicts with @property ownership modifier 'unowned'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-scalar-ownership",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_scalar_ownership_negative.objc3",
                expected_snippets=[
                    "@property ownership modifier 'strong' requires an Objective-C object property"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-duplicate-getter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "accessor_duplicate_getter_negative.objc3",
                expected_snippets=[
                    "duplicate effective getter selector 'value' for properties 'token' and 'alias'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-duplicate-setter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "accessor_duplicate_setter_negative.objc3",
                expected_snippets=[
                    "duplicate effective setter selector 'setValue:' for properties 'token' and 'alias'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="negative-readonly-setter",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_readonly_setter_negative.objc3",
                expected_snippets=[
                    "readonly property 'value' in interface 'Widget' must not declare a setter modifier"
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    storage_negative_results = {
        str(entry["key"]): entry for entry in negative_batch["results"]
    }

    return CaseResult(
        case_id="storage-legality-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/runtime_backed_storage_ownership_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "runtime_export_property_attribute_invalid_entries": sema_pass_manager_manifest.get(
                "runtime_export_property_attribute_invalid_entries"
            ),
            "runtime_export_property_attribute_contract_violations": sema_pass_manager_manifest.get(
                "runtime_export_property_attribute_contract_violations"
            ),
            "atomic_negative_diagnostic_count": storage_negative_results[
                "negative-atomic-ownership"
            ]["diagnostic_count"],
            "weak_mismatch_diagnostic_count": storage_negative_results[
                "negative-weak-mismatch"
            ]["diagnostic_count"],
            "unowned_mismatch_diagnostic_count": storage_negative_results[
                "negative-unowned-mismatch"
            ]["diagnostic_count"],
            "scalar_ownership_negative_diagnostic_count": storage_negative_results[
                "negative-scalar-ownership"
            ]["diagnostic_count"],
            "duplicate_getter_negative_diagnostic_count": storage_negative_results[
                "negative-duplicate-getter"
            ][
                "diagnostic_count"
            ],
            "duplicate_setter_negative_diagnostic_count": storage_negative_results[
                "negative-duplicate-setter"
            ][
                "diagnostic_count"
            ],
            "readonly_setter_negative_diagnostic_count": storage_negative_results[
                "negative-readonly-setter"
            ][
                "diagnostic_count"
            ],
            "negative_diagnostics_batch": negative_batch,
        },
    )


def check_property_synthesis_storage_binding_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-synthesis-storage-binding-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_synthesis_default_ivar_binding_no_redeclaration.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(
        isinstance(lowering_surface, dict),
        "expected property synthesis/storage-binding positive fixture to publish the lowering surface",
    )
    expect(
        lowering_surface.get("property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesis sites",
    )
    expect(
        lowering_surface.get("property_synthesis_default_ivar_bindings") == 2,
        "expected no-redeclaration property synthesis fixture to publish two default ivar bindings",
    )
    expect(
        lowering_surface.get("interface_owned_property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two interface-owned synthesis sites",
    )
    expect(
        lowering_surface.get("implementation_property_redeclaration_sites") == 0,
        "expected no-redeclaration property synthesis fixture to publish zero implementation redeclaration sites",
    )
    expect(
        lowering_surface.get("ivar_binding_resolved") == 2,
        "expected no-redeclaration property synthesis fixture to publish two resolved ivar bindings",
    )
    expect(
        lowering_surface.get("synthesized_accessor_owner_entries") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesized accessor owner entries",
    )
    expect(
        lowering_surface.get("synthesized_getter_entries") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesized getter entries",
    )
    expect(
        lowering_surface.get("synthesized_setter_entries") == 1,
        "expected no-redeclaration property synthesis fixture to publish one synthesized setter entry",
    )
    expect(
        lowering_surface.get("current_property_read_entries") == 2,
        "expected no-redeclaration property synthesis fixture to route both getters through current-property reads",
    )
    expect(
        lowering_surface.get("current_property_exchange_entries") == 1,
        "expected no-redeclaration property synthesis fixture to route the strong setter through current-property exchange",
    )
    expect(
        lowering_surface.get("current_property_write_entries") == 0,
        "expected no-redeclaration property synthesis fixture to avoid plain current-property writes for the strong setter path",
    )
    expect(
        lowering_surface.get("weak_current_property_load_entries") == 0
        and lowering_surface.get("weak_current_property_store_entries") == 0,
        "expected no-redeclaration property synthesis fixture to avoid weak helper selection",
    )
    expect(
        registration_manifest.get("property_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two ivar descriptors",
    )
    replay_key = manifest.get("lowering_property_synthesis_ivar_binding", {}).get(
        "replay_key", ""
    )
    for snippet, label in (
        (
            "interface_owned_property_synthesis_sites=2",
            "two interface-owned synthesis sites in the replay key",
        ),
        (
            "implementation_property_redeclaration_sites=0",
            "zero implementation redeclaration sites in the replay key",
        ),
        (
            "define void @objc3_method_Widget_instance_setCurrentValue_(i32 %arg0)",
            "the synthesized setter definition",
        ),
        (
            "call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
            "the runtime-backed setter exchange path",
        ),
    ):
        expect(
            snippet in (replay_key if "sites=" in snippet else ll_text),
            f"expected no-redeclaration property synthesis fixture to publish {label}",
        )

    incompatible_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_synthesis_default_ivar_binding_incompatible_redeclaration.objc3",
        case_dir / "negative-incompatible-redeclaration",
        expected_snippets=[
            "type mismatch: property synthesis for 'token' in implementation 'Widget' drifted from the interface default ivar binding",
            "type mismatch: incompatible property signature for 'token' in implementation 'Widget'",
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="property-synthesis-storage-binding-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_synthesis_sites": lowering_surface.get("property_synthesis_sites"),
            "interface_owned_property_synthesis_sites": lowering_surface.get(
                "interface_owned_property_synthesis_sites"
            ),
            "implementation_property_redeclaration_sites": lowering_surface.get(
                "implementation_property_redeclaration_sites"
            ),
            "synthesized_getter_entries": lowering_surface.get(
                "synthesized_getter_entries"
            ),
            "synthesized_setter_entries": lowering_surface.get(
                "synthesized_setter_entries"
            ),
            "current_property_exchange_entries": lowering_surface.get(
                "current_property_exchange_entries"
            ),
            "negative_incompatible_redeclaration_diagnostic_count": incompatible_negative[
                "diagnostic_count"
            ],
        },
    )


def check_property_reflection_accessor_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "property-reflection-accessor-compatibility-diagnostics"
    negative_batch = compile_negative_diagnostic_batch(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="accessor-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_accessor_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective getter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="setter-selector-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_setter_selector_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: effective setter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="reflection-attribute-mismatch",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "property_reflection_attribute_compatibility_negative.objc3",
                expected_snippets=[
                    "type mismatch: reflected property attribute and ownership profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    negative_results = {str(entry["key"]): entry for entry in negative_batch["results"]}

    return CaseResult(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        probe="compile-diagnostics",
        fixture="tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "getter_selector_negative_diagnostic_count": negative_results[
                "accessor-selector-mismatch"
            ][
                "diagnostic_count"
            ],
            "setter_selector_negative_diagnostic_count": negative_results[
                "setter-selector-mismatch"
            ][
                "diagnostic_count"
            ],
            "reflection_attribute_negative_diagnostic_count": negative_results[
                "reflection-attribute-mismatch"
            ][
                "diagnostic_count"
            ],
            "negative_diagnostics_batch": negative_batch,
        },
    )


def check_property_ivar_ordering_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-ivar-ordering-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_ivar_source_model_completion_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    surface = manifest.get("runtime_property_ivar_storage_accessor_source_surface", {})
    expect(
        surface.get("layout_init_order_field")
        == "Objc3PropertyDecl.executable_ivar_init_order_index",
        "expected property/ivar storage source surface to publish the init-order field",
    )
    expect(
        surface.get("layout_destroy_order_field")
        == "Objc3PropertyDecl.executable_ivar_destroy_order_index",
        "expected property/ivar storage source surface to publish the destruction-order field",
    )
    expect(
        surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected property/ivar storage source surface to publish the init/destroy ordering model",
    )

    property_records = manifest.get("runtime_metadata_source_records", {}).get(
        "properties", []
    )
    ivar_records = manifest.get("runtime_metadata_source_records", {}).get(
        "ivars", []
    )
    expect(
        isinstance(property_records, list) and property_records,
        "expected property ordering fixture to publish property source records",
    )
    expect(
        isinstance(ivar_records, list) and ivar_records,
        "expected property ordering fixture to publish ivar source records",
    )

    property_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in property_records
        if isinstance(record, dict)
    }
    ivar_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in ivar_records
        if isinstance(record, dict)
    }
    expected_property_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
        ("class-implementation", "Widget", "token", 0, 2),
        ("class-implementation", "Widget", "value", 1, 1),
        ("class-implementation", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_property_records:
        record = property_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_layout_slot_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve slot index {init_index}",
        )
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    expected_ivar_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_ivar_records:
        record = ivar_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    interface_property_records = [
        property_index[( "class-interface", "Widget", property_name)]
        for property_name in ("token", "value", "count")
    ]
    implementation_property_records = [
        property_index[( "class-implementation", "Widget", property_name)]
        for property_name in ("token", "value", "count")
    ]
    interface_init_order = [
        record.get("executable_ivar_init_order_index")
        for record in interface_property_records
    ]
    interface_destroy_order = [
        record.get("executable_ivar_destroy_order_index")
        for record in interface_property_records
    ]
    implementation_init_order = [
        record.get("executable_ivar_init_order_index")
        for record in implementation_property_records
    ]
    implementation_destroy_order = [
        record.get("executable_ivar_destroy_order_index")
        for record in implementation_property_records
    ]
    expect(
        interface_init_order == [0, 1, 2],
        "expected interface property init order to remain monotonic",
    )
    expect(
        interface_destroy_order == [2, 1, 0],
        "expected interface property destruction order to remain reverse-monotonic",
    )
    expect(
        implementation_init_order == [0, 1, 2],
        "expected implementation property init order to match interface ordering",
    )
    expect(
        implementation_destroy_order == [2, 1, 0],
        "expected implementation property destruction order to match interface reverse ordering",
    )

    return CaseResult(
        case_id="property-ivar-ordering-semantics",
        probe="compile-manifest-source-records",
        fixture="tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_record_count": len(property_records),
            "ivar_record_count": len(ivar_records),
            "token_destroy_order_index": property_index.get(
                ("class-interface", "Widget", "token"), {}
            ).get("executable_ivar_destroy_order_index"),
            "count_init_order_index": property_index.get(
                ("class-interface", "Widget", "count"), {}
            ).get("executable_ivar_init_order_index"),
            "interface_init_order": interface_init_order,
            "interface_destroy_order": interface_destroy_order,
            "implementation_init_order": implementation_init_order,
            "implementation_destroy_order": implementation_destroy_order,
        },
    )


def check_synthesized_accessor_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "synthesized-accessor-runtime"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "synthesized_accessor_probe.cpp"
    exe_path = case_dir / "synthesized_accessor_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "synthesized accessor runtime probe")

    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})
    enabled_entry = payload.get("enabled_entry", {})
    set_enabled_entry = payload.get("set_enabled_entry", {})
    value_entry = payload.get("value_entry", {})
    set_value_entry = payload.get("set_value_entry", {})

    expect(payload.get("widget_instance", 0) > 0, "expected synthesized-accessor runtime probe to allocate a positive Widget receiver")
    expect(payload.get("set_count_result") == 0, "expected synthesized-accessor count setter dispatch to return zero")
    expect(payload.get("count_value") == 37, "expected synthesized-accessor count getter to reload 37")
    expect(payload.get("set_enabled_result") == 0, "expected synthesized-accessor enabled setter dispatch to return zero")
    expect(payload.get("enabled_value") == 1, "expected synthesized-accessor enabled getter to reload 1")
    expect(payload.get("set_value_result") == 0, "expected synthesized-accessor value setter dispatch to return zero")
    expect(payload.get("value_result") == 55, "expected synthesized-accessor value getter to reload 55")

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected synthesized-accessor runtime probe to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected synthesized-accessor runtime probe to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected synthesized-accessor runtime probe to materialize the accessor selector surface")
    expect(selector_state.get("metadata_backed_selector_count", 0) >= 6, "expected synthesized-accessor runtime probe to preserve metadata-backed selectors")

    expected_entries = (
        (count_entry, "count", 0, "implementation:Widget::instance_method:count"),
        (set_count_entry, "setCount:", 1, "implementation:Widget::instance_method:setCount:"),
        (enabled_entry, "enabled", 0, "implementation:Widget::instance_method:enabled"),
        (set_enabled_entry, "setEnabled:", 1, "implementation:Widget::instance_method:setEnabled:"),
        (value_entry, "value", 0, "implementation:Widget::instance_method:value"),
        (set_value_entry, "setValue:", 1, "implementation:Widget::instance_method:setValue:"),
    )
    for entry, selector, parameter_count, owner_identity in expected_entries:
        expect(entry.get("found") == 1 and entry.get("resolved") == 1, f"expected {selector} cache entry to resolve live")
        expect(entry.get("selector") == selector, f"expected {selector} cache entry to preserve selector spelling")
        expect(entry.get("parameter_count") == parameter_count, f"expected {selector} cache entry to preserve parameter count {parameter_count}")
        expect(entry.get("resolved_class_name") == "Widget", f"expected {selector} cache entry to resolve against Widget")
        expect(entry.get("resolved_owner_identity") == owner_identity, f"expected {selector} cache entry to preserve owner identity")

    return CaseResult(
        case_id="synthesized-accessor-runtime",
        probe="tests/tooling/runtime/synthesized_accessor_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "widget_instance": payload["widget_instance"],
            "count_value": payload["count_value"],
            "enabled_value": payload["enabled_value"],
            "value_result": payload["value_result"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_accessor_storage_lowering_metadata_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "accessor-storage-lowering-metadata"

    def load_compile_artifacts(fixture_name: str, output_name: str) -> tuple[dict[str, Any], str]:
        fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name
        _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / output_name)
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        return manifest, ll_path.read_text(encoding="utf-8")

    def find_property_entry(entries: list[dict[str, Any]], owner_kind: str, owner_name: str, property_name: str) -> dict[str, Any]:
        for entry in entries:
            if (
                entry.get("owner_kind") == owner_kind
                and entry.get("owner_name") == owner_name
                and entry.get("property_name") == property_name
            ):
                return entry
        raise RuntimeError(
            f"missing property entry for {owner_kind}:{owner_name}:{property_name}"
        )

    def expect_property_lowering(
        manifest: dict[str, Any],
        owner_kind: str,
        owner_name: str,
        property_name: str,
        synthesizes_executable_accessors: bool,
        getter_helper_symbol: str,
        setter_helper_symbol: str,
    ) -> None:
        runtime_metadata_records = manifest.get("runtime_metadata_source_records", {})
        property_records = runtime_metadata_records.get("properties", [])
        expect(
            isinstance(property_records, list),
            "expected runtime metadata source records to publish property entries",
        )
        property_record = find_property_entry(
            property_records, owner_kind, owner_name, property_name
        )
        expect(
            property_record.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected runtime metadata property record to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected runtime metadata property record to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected runtime metadata property record to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

        source_graph = manifest.get("objc_executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = manifest.get("executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = (
                manifest.get("frontend", {})
                .get("pipeline", {})
                .get("semantic_surface", {})
                .get("objc_executable_metadata_source_graph", {})
            )
        property_nodes = source_graph.get("property_node_entries", [])
        expect(
            isinstance(property_nodes, list),
            "expected executable metadata source graph to publish property nodes",
        )
        property_node = find_property_entry(
            property_nodes, owner_kind, owner_name, property_name
        )
        expect(
            property_node.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected executable metadata property node to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected executable metadata property node to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected executable metadata property node to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

    synthesized_manifest, synthesized_ll = load_compile_artifacts(
        "synthesized_accessor_property_lowering_positive.objc3",
        "synthesized-accessors",
    )
    synthesized_lowering_surface = synthesized_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        isinstance(synthesized_lowering_surface, dict),
        "expected synthesized accessor lowering metadata fixture to publish the lowering surface",
    )
    expect(
        synthesized_lowering_surface.get("contract_id")
        == DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        "expected synthesized accessor lowering metadata fixture to preserve the lowering surface contract id",
    )
    expect(
        synthesized_lowering_surface.get("compile_manifest_artifact") == "module.manifest.json",
        "expected lowering surface to couple back to the compile manifest artifact",
    )
    expect(
        synthesized_lowering_surface.get("registration_manifest_artifact")
        == "module.runtime-registration-manifest.json",
        "expected lowering surface to couple back to the runtime registration manifest artifact",
    )
    expect(
        synthesized_lowering_surface.get("object_artifact") == "module.obj"
        and synthesized_lowering_surface.get("backend_artifact") == "module.ll",
        "expected lowering surface to couple back to the emitted object and LLVM IR artifacts",
    )
    expect(
        synthesized_lowering_surface.get(
            "runtime_property_ivar_storage_accessor_source_surface_contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected lowering surface to point back at the runtime property/ivar storage source surface",
    )
    expect(
        synthesized_lowering_surface.get(
            "storage_accessor_runtime_abi_surface_contract_id"
        )
        == RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "expected lowering surface to point at the storage/accessor runtime ABI surface",
    )
    expect(
        synthesized_lowering_surface.get("lowering_contract_source_path")
        == "native/objc3c/src/lower/objc3_lowering_contract.h",
        "expected lowering surface to publish the lowering contract source path",
    )
    expect(
        synthesized_lowering_surface.get("ir_emitter_source_path")
        == "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "expected lowering surface to publish the IR emitter source path",
    )
    expect(
        synthesized_lowering_surface.get("frontend_artifacts_source_path")
        == "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        "expected lowering surface to publish the frontend artifacts source path",
    )
    expect(
        synthesized_lowering_surface.get("runtime_source_path")
        == "native/objc3c/src/runtime/objc3_runtime.cpp",
        "expected lowering surface to publish the runtime source path",
    )
    expect(
        synthesized_lowering_surface.get("accessor_storage_lowering_metadata_model")
        == "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
        "expected lowering surface to publish the accessor-storage metadata model",
    )
    expect(
        synthesized_lowering_surface.get(
            "accessor_storage_lowering_helper_selection_model"
        )
        == "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        "expected lowering surface to publish the helper-selection model",
    )
    expect(
        synthesized_lowering_surface.get("authoritative_fixture_paths")
        == [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "expected lowering surface to publish the authoritative fixture set",
    )
    expect(
        synthesized_lowering_surface.get("authoritative_probe_paths")
        == [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "expected lowering surface to publish the authoritative probe set",
    )
    expect(
        synthesized_lowering_surface.get("explicit_non_goals")
        == [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-lowering-proof",
        ],
        "expected lowering surface to publish explicit non-goals",
    )
    expect(
        synthesized_lowering_surface.get("requires_coupled_registration_manifest")
        is True
        and synthesized_lowering_surface.get("requires_real_compile_output") is True
        and synthesized_lowering_surface.get("requires_linked_runtime_probe") is True,
        "expected lowering surface to require the coupled registration manifest, real compile output, and linked runtime probes",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_accessor_owner_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered property owners",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered getters",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered setters",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three current-property read entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_entries") == 2,
        "expected synthesized accessor lowering metadata fixture to publish two current-property write entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected synthesized accessor lowering metadata fixture to publish one current-property exchange entry",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-load entries",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-store entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_symbol")
        == "objc3_runtime_read_current_property_i32",
        "expected lowering surface to publish the canonical current-property read symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_symbol")
        == "objc3_runtime_write_current_property_i32",
        "expected lowering surface to publish the canonical current-property write symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_symbol")
        == "objc3_runtime_exchange_current_property_i32",
        "expected lowering surface to publish the canonical current-property exchange symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_symbol")
        == "objc3_runtime_load_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property load symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_symbol")
        == "objc3_runtime_store_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property store symbol",
    )
    expect(
        "getter_definitions=3" in synthesized_ll
        and "setter_definitions=3" in synthesized_ll
        and "read_current_property_calls=3" in synthesized_ll
        and "write_current_property_calls=2" in synthesized_ll
        and "exchange_current_property_calls=1" in synthesized_ll,
        "expected synthesized accessor lowering metadata fixture LLVM IR to agree with the published lowering counts",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-interface",
        "Widget",
        "count",
        False,
        "",
        "",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "count",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "enabled",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "value",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )

    arc_manifest, arc_ll = load_compile_artifacts(
        "arc_property_interaction_positive.objc3",
        "arc-accessors",
    )
    arc_lowering_surface = arc_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        arc_lowering_surface.get("synthesized_accessor_owner_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered property owners",
    )
    expect(
        arc_lowering_surface.get("synthesized_getter_entries") == 2
        and arc_lowering_surface.get("synthesized_setter_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered getters and setters",
    )
    expect(
        arc_lowering_surface.get("current_property_read_entries") == 1,
        "expected ARC property interaction fixture to publish one plain/strong getter read entry",
    )
    expect(
        arc_lowering_surface.get("current_property_write_entries") == 0,
        "expected ARC property interaction fixture to publish zero plain write entries",
    )
    expect(
        arc_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected ARC property interaction fixture to publish one strong exchange entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_load_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-load entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_store_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-store entry",
    )
    expect(
        "exchange_current_property_calls=1" in arc_ll
        and "weak_load_current_property_calls=1" in arc_ll
        and "weak_store_current_property_calls=1" in arc_ll,
        "expected ARC property interaction fixture LLVM IR to agree with the published helper-lowering counts",
    )
    expect_property_lowering(
        arc_manifest,
        "class-interface",
        "ArcBox",
        "currentValue",
        False,
        "",
        "",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "currentValue",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "weakValue",
        True,
        "objc3_runtime_load_weak_current_property_i32",
        "objc3_runtime_store_weak_current_property_i32",
    )

    return CaseResult(
        case_id="accessor-storage-lowering-metadata-surface",
        probe="compile-manifest-and-executable-metadata-surface",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "synthesized_accessor_owner_entries": synthesized_lowering_surface.get(
                "synthesized_accessor_owner_entries"
            ),
            "synthesized_getter_entries": synthesized_lowering_surface.get(
                "synthesized_getter_entries"
            ),
            "synthesized_setter_entries": synthesized_lowering_surface.get(
                "synthesized_setter_entries"
            ),
            "strong_exchange_entries": synthesized_lowering_surface.get(
                "current_property_exchange_entries"
            ),
            "weak_store_entries": arc_lowering_surface.get(
                "weak_current_property_store_entries"
            ),
        },
    )


def check_property_accessor_layout_lowering_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-accessor-layout-lowering"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "synthesized_accessor_property_lowering_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = (
        case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    ll_text = ll_path.read_text(encoding="utf-8")

    property_source_surface = manifest.get(
        "runtime_property_ivar_storage_accessor_source_surface", {}
    )
    expect(
        property_source_surface.get(
            "executable_property_accessor_layout_lowering_contract_id"
        )
        == "objc3c.executable.property.accessor.layout.lowering.v1",
        "expected property/ivar storage source surface to point at the executable accessor/layout lowering surface",
    )
    expect(
        property_source_surface.get("executable_ivar_layout_emission_contract_id")
        == "objc3c.executable.ivar.layout.emission.v1",
        "expected property/ivar storage source surface to point at the executable ivar layout emission surface",
    )
    expect(
        property_source_surface.get(
            "executable_synthesized_accessor_property_lowering_contract_id"
        )
        == "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "expected property/ivar storage source surface to point at the synthesized accessor lowering surface",
    )

    accessor_layout_surface = manifest.get(
        "executable_property_accessor_layout_lowering_surface", {}
    )
    expect(
        isinstance(accessor_layout_surface, dict),
        "expected compile manifest to publish the executable accessor/layout lowering surface",
    )
    expected_accessor_layout_fields = {
        "contract_id": "objc3c.executable.property.accessor.layout.lowering.v1",
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "scope_model": "ast-sema-property-layout-handoff-ir-object-metadata-publication",
        "fail_closed_model": (
            "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation"
        ),
        "compile_manifest_artifact": "module.manifest.json",
        "registration_manifest_artifact": "module.runtime-registration-manifest.json",
        "object_artifact": "module.obj",
        "backend_artifact": "module.ll",
    }
    for field, expected_value in expected_accessor_layout_fields.items():
        expect(
            accessor_layout_surface.get(field) == expected_value,
            f"expected accessor/layout lowering surface to preserve {field}",
        )
    expect(
        accessor_layout_surface.get("property_metadata_entries") == 6,
        "expected accessor/layout lowering surface to publish six property metadata entries",
    )
    expect(
        accessor_layout_surface.get("ivar_metadata_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar metadata entries",
    )
    expect(
        accessor_layout_surface.get("property_descriptor_entries") == 6,
        "expected accessor/layout lowering surface to publish six property descriptors",
    )
    expect(
        accessor_layout_surface.get("ivar_descriptor_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar descriptors",
    )
    expect(
        accessor_layout_surface.get("property_attribute_profile_entries") == 6,
        "expected accessor/layout lowering surface to publish six property attribute profiles",
    )
    expect(
        accessor_layout_surface.get("accessor_ownership_profile_entries") == 6,
        "expected accessor/layout lowering surface to publish six accessor ownership profiles",
    )
    expect(
        accessor_layout_surface.get("synthesized_binding_entries") == 6,
        "expected accessor/layout lowering surface to publish six synthesized binding entries",
    )
    expect(
        accessor_layout_surface.get("ivar_layout_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar layout entries",
    )
    expect(
        accessor_layout_surface.get("ivar_layout_owner_entries") == 1,
        "expected accessor/layout lowering surface to publish one ivar layout owner",
    )
    expect(
        accessor_layout_surface.get("descriptor_counts_match_source_graph") is True,
        "expected accessor/layout lowering surface descriptor counts to match the executable source graph",
    )

    ivar_layout_surface = manifest.get("executable_ivar_layout_emission_surface", {})
    expect(
        isinstance(ivar_layout_surface, dict),
        "expected compile manifest to publish the executable ivar layout emission surface",
    )
    expected_ivar_layout_fields = {
        "contract_id": "objc3c.executable.ivar.layout.emission.v1",
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            "objc3c.executable.property.accessor.layout.lowering.v1"
        ),
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering"
        ),
        "offset_global_model": "one-retained-i64-offset-global-per-emitted-ivar-binding",
        "layout_table_model": (
            "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size"
        ),
        "scope_model": (
            "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation"
        ),
        "fail_closed_model": (
            "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis"
        ),
    }
    for field, expected_value in expected_ivar_layout_fields.items():
        expect(
            ivar_layout_surface.get(field) == expected_value,
            f"expected ivar layout emission surface to preserve {field}",
        )
    expect(
        ivar_layout_surface.get("offset_global_entries") == 3,
        "expected ivar layout emission surface to publish three offset globals",
    )
    expect(
        ivar_layout_surface.get("layout_table_entries") == 1,
        "expected ivar layout emission surface to publish one layout table",
    )
    expect(
        ivar_layout_surface.get("layout_owner_entries") == 1,
        "expected ivar layout emission surface to publish one layout owner",
    )
    expect(
        ivar_layout_surface.get("ivar_descriptor_entries") == 3,
        "expected ivar layout emission surface to publish three ivar descriptors",
    )

    synthesized_accessor_surface = manifest.get(
        "executable_synthesized_accessor_property_lowering_surface", {}
    )
    expect(
        isinstance(synthesized_accessor_surface, dict),
        "expected compile manifest to publish the synthesized accessor lowering surface",
    )
    expected_synthesized_accessor_fields = {
        "contract_id": "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            "objc3c.executable.property.accessor.layout.lowering.v1"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "source_model": (
            "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists"
        ),
        "storage_model": (
            "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals"
        ),
        "property_descriptor_model": (
            "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers"
        ),
        "fail_closed_model": (
            "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-fallbacks"
        ),
    }
    for field, expected_value in expected_synthesized_accessor_fields.items():
        expect(
            synthesized_accessor_surface.get(field) == expected_value,
            f"expected synthesized accessor lowering surface to preserve {field}",
        )
    expect(
        synthesized_accessor_surface.get("implementation_owned_property_entries") == 3,
        "expected synthesized accessor lowering surface to publish three implementation-owned properties",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering surface to publish three synthesized getters",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering surface to publish three synthesized setters",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_accessor_entries") == 6,
        "expected synthesized accessor lowering surface to publish six synthesized accessors",
    )
    expect(
        synthesized_accessor_surface.get("property_descriptor_entries") == 6,
        "expected synthesized accessor lowering surface to publish six property descriptors",
    )

    expect(
        registration_manifest.get("property_descriptor_count") == 6,
        "expected registration manifest to publish six property descriptors for the synthesized accessor lowering fixture",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 3,
        "expected registration manifest to publish three ivar descriptors for the synthesized accessor lowering fixture",
    )

    for snippet, label in (
        (
            "; executable_property_accessor_layout_lowering = "
            "contract=objc3c.executable.property.accessor.layout.lowering.v1",
            "the executable property accessor/layout lowering summary",
        ),
        (
            "property_metadata_entries=6;ivar_metadata_entries=3;"
            "property_attribute_profiles=6;accessor_ownership_profiles=6;"
            "synthesized_binding_entries=6;ivar_layout_entries=3",
            "the accessor/layout lowering inventory counts",
        ),
        (
            "; executable_ivar_layout_emission = "
            "contract=objc3c.executable.ivar.layout.emission.v1",
            "the executable ivar layout emission summary",
        ),
        (
            "offset_global_entries=3;layout_table_entries=1;layout_owner_entries=1",
            "the ivar layout emission inventory counts",
        ),
        (
            "; executable_synthesized_accessor_property_lowering = "
            "contract=objc3c.executable.synthesized.accessor.property.lowering.v1",
            "the synthesized accessor lowering summary",
        ),
        (
            "synthesized_accessor_entries=6",
            "the synthesized accessor entry count",
        ),
        (
            "define i32 @objc3_method_Widget_instance_count() {",
            "the synthesized count getter body",
        ),
        (
            "define void @objc3_method_Widget_instance_setCount_(i32 %arg0) {",
            "the synthesized count setter body",
        ),
        (
            "@__objc3_meta_ivar_layout_table_0000 = private global",
            "the emitted ivar layout table",
        ),
        (
            "@__objc3_meta_ivar_offset_0000",
            "the emitted ivar offset globals",
        ),
    ):
        expect(
            snippet in ll_text,
            f"expected synthesized accessor/layout lowering fixture LLVM IR to publish {label}",
        )

    return CaseResult(
        case_id="property-accessor-layout-lowering",
        probe="compile-manifest-registration-manifest-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_descriptor_entries": accessor_layout_surface.get(
                "property_descriptor_entries"
            ),
            "ivar_descriptor_entries": accessor_layout_surface.get(
                "ivar_descriptor_entries"
            ),
            "synthesized_accessor_entries": synthesized_accessor_surface.get(
                "synthesized_accessor_entries"
            ),
            "layout_table_entries": ivar_layout_surface.get("layout_table_entries"),
        },
    )


def check_property_layout_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-layout"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path, ll_path, _ = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "property_layout_runtime_probe.cpp"
    exe_path = case_dir / "property_layout_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property layout runtime probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})

    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime property/layout consumption surface",
    )
    expect("synthesized_accessor_entries=6" in ll_text, "expected property-layout fixture to preserve six synthesized accessors")
    expect("property_descriptor_entries=" in ll_text, "expected property-layout fixture to publish property descriptor inventory")
    expect("ivar_layout_owner_entries=" in ll_text, "expected property-layout fixture to publish ivar layout owner inventory")

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))

    expect(first_alloc > 0, "expected first alloc to materialize a positive Widget instance identity")
    expect(second_alloc > 0, "expected second alloc to materialize a positive Widget instance identity")
    expect(
        first_alloc != second_alloc,
        "expected property-layout runtime to allocate distinct Widget instance identities",
    )
    expect(payload.get("set_count_result") == 0, "expected count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected count getter to observe the written value on the first alloc")
    expect(
        payload.get("count_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance count storage",
    )
    expect(payload.get("set_enabled_result") == 0, "expected enabled setter dispatch to return zero")
    expect(
        payload.get("enabled_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance enabled storage",
    )
    expect(payload.get("set_value_result") == 0, "expected value setter dispatch to return zero")
    expect(
        payload.get("value_result_second") == 0,
        "expected second alloc to observe zero-filled per-instance strong value storage",
    )

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected property-layout runtime to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected property-layout runtime to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected property-layout runtime to materialize the synthesized accessor selector surface")

    expect(count_entry.get("found") == 1 and count_entry.get("resolved") == 1, "expected count getter cache entry to resolve")
    expect(count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        str(count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:count"),
        "expected count getter cache entry to preserve the synthesized owner identity",
    )

    expect(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1, "expected setCount setter cache entry to resolve")
    expect(set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        str(set_count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:setCount:"),
        "expected setCount setter cache entry to preserve the synthesized owner identity",
    )

    return CaseResult(
        case_id="property-layout",
        probe="tests/tooling/runtime/property_layout_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "count_value_first": payload["count_value_first"],
            "count_value_second": payload["count_value_second"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_instance_allocation_layout_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "instance-allocation-layout-runtime"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "instance_allocation_runtime_probe.cpp"
    exe_path = case_dir / "instance_allocation_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "instance allocation runtime probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    graph_state = payload.get("graph_state", {})
    widget_entry = payload.get("widget_entry", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})

    expect(
        "; runtime_instance_allocation_layout_support = "
        "contract=objc3c.runtime.instance.allocation.layout.support.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime instance allocation/layout support surface",
    )
    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in ll_text,
        "expected LLVM IR to preserve the property/layout consumption surface coupled to instance allocation",
    )
    expect("synthesized_accessor_entries=6" in ll_text, "expected instance allocation fixture to preserve six synthesized accessors")
    expect("property_descriptor_entries=" in ll_text, "expected instance allocation fixture to publish property descriptors")
    expect("ivar_layout_owner_entries=" in ll_text, "expected instance allocation fixture to publish ivar layout owners")

    runtime_surface = manifest.get("runtime_property_ivar_storage_accessor_source_surface", {})
    expect(
        runtime_surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected compile manifest to keep the source layout model coupled to runtime allocation",
    )
    synthesized_surface = manifest.get("executable_synthesized_accessor_property_lowering_surface", {})
    expect(
        synthesized_surface.get("storage_model")
        == "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals",
        "expected synthesized accessor lowering to route storage through runtime helpers",
    )

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))
    expect(first_alloc == 1048576, "expected first runtime instance identity to start at 1048576")
    expect(second_alloc == 1048577, "expected second runtime instance identity to increment deterministically")
    expect(first_alloc != second_alloc, "expected alloc to materialize distinct receiver identities")

    expect(payload.get("set_count_first") == 0, "expected first count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected first count getter to read its written value")
    expect(payload.get("count_value_second_before") == 0, "expected second count getter to start from zero-filled storage")
    expect(payload.get("set_enabled_first") == 0, "expected first enabled setter dispatch to return zero")
    expect(payload.get("enabled_value_first") == 1, "expected first enabled getter to read its written value")
    expect(payload.get("enabled_value_second") == 0, "expected second enabled getter to start from zero-filled storage")
    expect(payload.get("set_value_first") == 0, "expected first strong value setter dispatch to return zero")
    expect(payload.get("value_result_first") == 55, "expected first strong value getter to read its retained slot value")
    expect(payload.get("value_result_second_before") == 0, "expected second strong value getter to start from nil/zero storage")
    expect(payload.get("set_count_second") == 0, "expected second count setter dispatch to return zero")
    expect(payload.get("count_value_first_after_second") == 37, "expected second count write not to affect the first instance")
    expect(payload.get("count_value_second_after") == 9, "expected second count getter to read its own written value")
    expect(payload.get("set_value_second") == 0, "expected second strong value setter dispatch to return zero")
    expect(payload.get("value_result_first_after_second") == 55, "expected second value write not to affect the first instance")
    expect(payload.get("value_result_second_after") == 91, "expected second value getter to read its own written value")

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected registered image state for instance allocation probe")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected registered descriptors for instance allocation probe")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected synthesized accessor selectors in the selector table")
    expect(selector_state.get("metadata_backed_selector_count", 0) >= 6, "expected selector table to distinguish metadata-backed selectors")

    expect(graph_state.get("realized_class_count") == 1, "expected one realized Widget class")
    expect(graph_state.get("root_class_count") == 1, "expected Widget to be realized as a root class")
    expect(graph_state.get("receiver_class_binding_count") == 1, "expected one class receiver binding")
    expect(graph_state.get("live_instance_count") == 2, "expected two live runtime instances")
    expect(graph_state.get("last_allocated_receiver_identity") == second_alloc, "expected graph state to record the last allocated receiver")
    expect(graph_state.get("last_allocated_base_identity") == 1024, "expected graph state to record the Widget class base identity")
    expect(graph_state.get("last_allocated_instance_size_bytes") == 16, "expected Widget instance storage size to remain 16 bytes")
    expect(graph_state.get("last_allocated_class_name") == "Widget", "expected graph state to record the allocated class name")

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be queryable")
    expect(widget_entry.get("base_identity") == 1024, "expected Widget base identity to remain deterministic")
    expect(widget_entry.get("is_root_class") == 1, "expected Widget fixture to be a root class")
    expect(widget_entry.get("implementation_backed") == 1, "expected Widget entry to be implementation backed")
    expect(widget_entry.get("runtime_property_accessor_count") == 3, "expected Widget to publish three runtime-backed property accessors")
    expect(widget_entry.get("runtime_instance_size_bytes") == 16, "expected Widget entry to publish 16 bytes of instance storage")
    expect(widget_entry.get("class_owner_identity") == "class:Widget", "expected Widget class owner identity")
    expect(widget_entry.get("metaclass_owner_identity") == "metaclass:Widget", "expected Widget metaclass owner identity")

    expect(count_entry.get("found") == 1 and count_entry.get("resolved") == 1, "expected count getter cache entry to resolve")
    expect(count_entry.get("dispatch_family_is_class") == 0, "expected count getter dispatch to be instance-family")
    expect(count_entry.get("normalized_receiver_identity") == 1025, "expected instance dispatch to normalize to Widget instance identity")
    expect(count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        count_entry.get("resolved_owner_identity") == "implementation:Widget::instance_method:count",
        "expected count getter cache entry to preserve synthesized owner identity",
    )
    expect(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1, "expected setCount setter cache entry to resolve")
    expect(set_count_entry.get("dispatch_family_is_class") == 0, "expected setCount setter dispatch to be instance-family")
    expect(set_count_entry.get("normalized_receiver_identity") == 1025, "expected setter dispatch to normalize to Widget instance identity")
    expect(set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        set_count_entry.get("resolved_owner_identity") == "implementation:Widget::instance_method:setCount:",
        "expected setCount setter cache entry to preserve synthesized owner identity",
    )

    return CaseResult(
        case_id="instance-allocation-layout-runtime",
        probe="tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "live_instance_count": graph_state.get("live_instance_count"),
            "instance_size_bytes": graph_state.get("last_allocated_instance_size_bytes"),
            "widget_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "count_first_after_second": payload["count_value_first_after_second"],
            "count_second_after": payload["count_value_second_after"],
        },
    )


def check_synthesized_accessor_codegen_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-codegen"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    required_ir_snippets = {
        "count getter": "define i32 @objc3_method_Widget_instance_count()",
        "count setter": "define void @objc3_method_Widget_instance_setCount_(i32 %arg0)",
        "enabled getter": "define i1 @objc3_method_Widget_instance_enabled()",
        "enabled setter": "define void @objc3_method_Widget_instance_setEnabled_(i1 %arg0)",
        "value getter": "define i32 @objc3_method_Widget_instance_value()",
        "value setter": "define void @objc3_method_Widget_instance_setValue_(i32 %arg0)",
        "getter runtime read": "call i32 @objc3_runtime_read_current_property_i32()",
        "setter runtime write": "call void @objc3_runtime_write_current_property_i32(i32 %arg0)",
        "bool setter coercion": "%objc3_property_value = zext i1 %arg0 to i32",
        "strong getter retain": "%objc3_property_retained = call i32 @objc3_runtime_retain_i32(i32 %objc3_property_slot)",
        "strong getter autorelease": "%objc3_property_autoreleased = call i32 @objc3_runtime_autorelease_i32(i32 %objc3_property_retained)",
        "strong setter exchange": "%objc3_property_previous = call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
        "strong setter release": "%objc3_property_release = call i32 @objc3_runtime_release_i32(i32 %objc3_property_previous)",
        "count descriptor getter binding": "ptr @objc3_method_Widget_instance_count, ptr @objc3_method_Widget_instance_setCount_",
        "enabled descriptor getter binding": "ptr @objc3_method_Widget_instance_enabled, ptr @objc3_method_Widget_instance_setEnabled_",
        "value descriptor getter binding": "ptr @objc3_method_Widget_instance_value, ptr @objc3_method_Widget_instance_setValue_",
    }
    for label, snippet in required_ir_snippets.items():
        expect(snippet in ll_text, f"expected synthesized accessor codegen to emit {label}")

    synthesis_summary = manifest.get("lowering_property_synthesis_ivar_binding", {})
    expect(isinstance(synthesis_summary, dict), "expected property synthesis lowering summary in compile manifest")
    expect(synthesis_summary.get("deterministic_handoff") is True,
           "expected property synthesis lowering summary to report deterministic handoff")
    replay_key = synthesis_summary.get("replay_key", "")
    expect("property_synthesis_sites=3" in replay_key,
           "expected property synthesis replay key to record the three synthesized properties")
    expect("property_synthesis_default_ivar_bindings=3" in replay_key,
           "expected property synthesis replay key to record the default ivar bindings")
    expect(registration_manifest.get("property_descriptor_count", 0) >= 6,
           "expected runtime registration manifest to publish synthesized property descriptors")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected authoritative dispatch and synthesized-accessor lowering surface in compile manifest")
    expect(lowering_surface.get("contract_id") ==
           "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
           "expected lowering surface contract id for dispatch and synthesized accessors")
    expect(lowering_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected lowering surface to publish canonical runtime dispatch symbol")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected lowering surface to bind lowering and runtime library dispatch symbols together")
    expect(lowering_surface.get("property_synthesis_sites") == 3,
           "expected lowering surface to publish three synthesized properties")
    expect(lowering_surface.get("property_synthesis_default_ivar_bindings") == 3,
           "expected lowering surface to publish three default ivar bindings")
    expect(lowering_surface.get("property_descriptor_count") == registration_manifest.get("property_descriptor_count"),
           "expected lowering surface property descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("ivar_descriptor_count") == registration_manifest.get("ivar_descriptor_count"),
           "expected lowering surface ivar descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("deterministic_handoff") is True,
           "expected lowering surface to report deterministic handoff")
    expect(
        "; dispatch_and_synthesized_accessor_lowering_surface = "
        "contract_id=objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        in ll_text,
        "expected LLVM IR banner to publish dispatch and synthesized-accessor lowering surface",
    )
    expect("property_synthesis_sites=3" in ll_text,
           "expected LLVM IR banner to report three synthesized properties")
    expect("property_descriptor_count=6" in ll_text,
           "expected LLVM IR banner to report synthesized property descriptor count")
    expect("member_table_emission_ready=true" in ll_text,
           "expected LLVM IR banner to report member table emission readiness")
    expect(
        "; synthesized_getter_setter_llvm_ir_generation_surface = "
        "contract_id=objc3c.synthesized.getter.setter.llvm.ir.generation.v1"
        in ll_text,
        "expected LLVM IR to publish synthesized getter/setter generation surface",
    )
    expect("getter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three getter definitions")
    expect("setter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three setter definitions")
    expect("read_current_property_calls=3" in ll_text,
           "expected synthesized accessor fixture to emit three current-property reads")
    expect("write_current_property_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two current-property writes")
    expect("exchange_current_property_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one strong current-property exchange")
    expect("retain_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two retain helper calls")
    expect("release_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one release helper call")
    expect("autorelease_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one autorelease helper call")

    return CaseResult(
        case_id="property-codegen",
        probe="real-compile-llvm-inspection",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        },
    )
