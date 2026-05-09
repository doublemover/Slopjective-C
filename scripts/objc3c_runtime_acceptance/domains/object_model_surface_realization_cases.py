"""Object Model realization and lowering acceptance surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..core import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_object_model_realization_source_surface",
    "build_runtime_realization_lowering_reflection_artifact_surface",
    "build_runtime_dispatch_table_reflection_record_lowering_surface",
    "build_runtime_cross_module_realized_metadata_replay_preservation_surface",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def build_runtime_object_model_realization_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "multi-image-registration-reset-replay",
            "canonical-dispatch",
            "metaclass-graph-root-class",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.executable.realization.records.v1",
            "objc3c.runtime.class.realization.freeze.v1",
            "objc3c.runtime.metaclass.graph.root.class.baseline.v1",
            "objc3c.runtime.category.attachment.protocol.conformance.v1",
            "objc3c.runtime.canonical.runnable.object.sample.support.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_object_model_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/metaclass_graph_root_class_library.objc3",
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/metaclass_graph_root_class_probe.cpp",
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lowering_reflection_artifact_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.compile-provenance.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
            "objc3c.executable.realization.records.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "lowering_artifact_boundary_model": (
            "compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts"
        ),
        "reflection_artifact_handoff_model": (
            "property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_reflection_artifact_query_boundary": [
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def build_runtime_dispatch_table_reflection_record_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
            "property-execution",
        }
    ]
    return {
        "contract_id": RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.compile-provenance.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
            "objc3c.method.dispatch.selector.thunk.lowering.v1",
            "objc3c.executable.realization.records.v1",
        ],
        "dispatch_table_lowering_model": (
            "selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts"
        ),
        "reflection_record_lowering_model": (
            "realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def build_runtime_cross_module_realized_metadata_replay_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "imported-runtime-packaging-replay",
            "multi-image-registration-reset-replay",
        }
    ]
    return {
        "contract_id": RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.cross-module-runtime-linker-options.rsp",
        ],
        "source_contract_ids": [
            "objc3c.cross.module.runtime.packaging.link.plan.v1",
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "realized_metadata_replay_preservation_model": (
            "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
