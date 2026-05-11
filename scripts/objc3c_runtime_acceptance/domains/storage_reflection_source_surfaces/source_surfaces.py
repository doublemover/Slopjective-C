"""Storage/reflection source-surface definitions."""

from __future__ import annotations

from ...runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)
from .case_catalog import (
    PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_CASE_IDS,
    PROPERTY_IVAR_STORAGE_ACCESSOR_CASE_IDS,
)
from .constants import (
    COMPILE_ARTIFACT_SET,
    REQUIRES_COUPLED_REGISTRATION_MANIFEST,
    REQUIRES_LINKED_RUNTIME_PROBE,
    REQUIRES_REAL_COMPILE_OUTPUT,
)
from .models import StorageReflectionSourceSurfaceDefinition


PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE = (
    StorageReflectionSourceSurfaceDefinition(
        contract_id=RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        case_ids=PROPERTY_IVAR_STORAGE_ACCESSOR_CASE_IDS,
        pre_case_fields=(
            ("compile_artifact_set", COMPILE_ARTIFACT_SET),
            (
                "source_contract_ids",
                (
                    "objc3c.executable.property.ivar.source.closure.v1",
                    "objc3c.executable.property.ivar.source.model.completion.v1",
                    "objc3c.executable.property.ivar.semantics.v1",
                ),
            ),
            (
                "authoritative_code_paths",
                (
                    "native/objc3c/src/ast/objc3_ast.h",
                    "native/objc3c/src/lower/objc3_lowering_contract.h",
                    "native/objc3c/src/sema/objc3_semantic_passes.cpp",
                    "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
                    "native/objc3c/src/ir/objc3_ir_emitter.cpp",
                    "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
                    "native/objc3c/src/runtime/objc3_runtime.cpp",
                ),
            ),
            (
                "authoritative_source_fields",
                (
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
                ),
            ),
            (
                "semantic_boundary_model",
                "property-ivar-storage-accessor-source-surface-freezes-ast-sema-ir-pipeline-and-runtime-codepaths-before-lowering-or-runtime-semantic-expansion",
            ),
            (
                "source_models",
                (
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
                ),
            ),
        ),
        fixture_paths=(
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ),
        probe_paths=(
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ),
        post_probe_fields=(
            (
                "explicit_non_goals",
                (
                    "no-public-runtime-abi-widening",
                    "no-milestone-specific-scaffolding",
                    "no-lowering-owned-storage-or-accessor-semantics-invention",
                ),
            ),
            (
                "requires_coupled_registration_manifest",
                REQUIRES_COUPLED_REGISTRATION_MANIFEST,
            ),
            ("requires_real_compile_output", REQUIRES_REAL_COMPILE_OUTPUT),
            ("requires_linked_runtime_probe", REQUIRES_LINKED_RUNTIME_PROBE),
        ),
    )
)


PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE = (
    StorageReflectionSourceSurfaceDefinition(
        contract_id=(
            RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID
        ),
        case_ids=PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_CASE_IDS,
        pre_case_fields=(
            ("compile_artifact_set", COMPILE_ARTIFACT_SET),
            (
                "source_contract_ids",
                (
                    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
                    "objc3c.runtime.property.metadata.reflection.v1",
                    "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
                ),
            ),
            (
                "authoritative_code_paths",
                (
                    "native/objc3c/src/ast/objc3_ast.h",
                    "native/objc3c/src/sema/objc3_semantic_passes.cpp",
                    "native/objc3c/src/sema/objc3_sema_pass_manager.cpp",
                    "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
                    "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
                    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
                    "native/objc3c/src/runtime/objc3_runtime.cpp",
                ),
            ),
            (
                "authoritative_source_fields",
                (
                    "Objc3PropertyDecl.is_atomic",
                    "Objc3PropertyDecl.is_nonatomic",
                    "Objc3PropertyDecl.has_atomicity_conflict",
                    "Objc3PropertyDecl.property_attribute_profile",
                    "objc3_runtime_property_entry_snapshot.property_attribute_profile",
                ),
            ),
            (
                "source_surface_model",
                "property-atomicity-synthesis-reflection-source-surface-freezes-atomicity-flags-conflict-state-attribute-profiles-and-private-reflection-codepaths-before-runtime-managed-atomic-storage-semantics-land",
            ),
            (
                "atomicity_fail_closed_model",
                "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land",
            ),
            (
                "reflection_boundary_model",
                "property-attribute-profiles-remain-the-authoritative-reflection-carrier-for-atomicity-and-synthesis-state-on-the-private-property-query-boundary",
            ),
        ),
        fixture_paths=(
            "tests/tooling/fixtures/native/property_atomic_ownership_negative.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ),
        probe_paths=(
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ),
        post_probe_fields=(
            (
                "explicit_non_goals",
                (
                    "no-public-atomic-property-runtime-abi-widening",
                    "no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation",
                    "no-milestone-specific-scaffolding",
                ),
            ),
            (
                "requires_coupled_registration_manifest",
                REQUIRES_COUPLED_REGISTRATION_MANIFEST,
            ),
            ("requires_real_compile_output", REQUIRES_REAL_COMPILE_OUTPUT),
            ("requires_linked_runtime_probe", REQUIRES_LINKED_RUNTIME_PROBE),
        ),
    )
)


__all__ = [
    "PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE",
    "PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE",
]
