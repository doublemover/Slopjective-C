"""Object Model runtime acceptance domain."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..core import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_object_model_realization_source_surface",
    "build_runtime_realization_lowering_reflection_artifact_surface",
    "build_runtime_dispatch_table_reflection_record_lowering_surface",
    "build_runtime_cross_module_realized_metadata_replay_preservation_surface",
    "build_runtime_object_model_abi_query_surface",
    "build_runtime_realization_lookup_reflection_implementation_surface",
    "build_runtime_reflection_query_surface",
    "build_runtime_realization_lookup_semantics_surface",
    "build_runtime_class_metaclass_protocol_realization_surface",
    "build_runtime_category_attachment_merged_dispatch_surface",
    "build_runtime_reflection_visibility_coherence_diagnostics_surface",
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


def build_runtime_object_model_abi_query_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "metaclass-graph-root-class",
            "canonical-sample-set",
            "realization-lookup-reflection-runtime",
            "dispatch-fast-path",
            "instance-allocation-layout-runtime",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ],
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_object_model_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_selector_lookup_table_state_for_testing",
            "objc3_runtime_copy_selector_lookup_entry_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
            "objc3_runtime_copy_dispatch_state_for_testing",
        ],
        "object_model_query_boundary_model": (
            "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/metaclass_graph_root_class_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/metaclass_graph_root_class_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lookup_reflection_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "realization-lookup-reflection-runtime",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ],
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "object_model_query_state_snapshot_symbol": (
            "objc3_runtime_copy_object_model_query_state_for_testing"
        ),
        "realized_class_entry_snapshot_symbol": (
            "objc3_runtime_copy_realized_class_entry_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "protocol_conformance_query_symbol": (
            "objc3_runtime_copy_protocol_conformance_query_for_testing"
        ),
        "selector_lookup_table_state_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_table_state_for_testing"
        ),
        "selector_lookup_entry_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_entry_for_testing"
        ),
        "method_cache_state_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_state_for_testing"
        ),
        "method_cache_entry_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_entry_for_testing"
        ),
        "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
        "realization_lookup_reflection_implementation_model": (
            "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_reflection_query_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-sample-set",
            "property-reflection",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "query_api_boundary_model": (
            "private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_query_symbols": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "no_public_reflection_abi": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lookup_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_lookup_query_boundary": [
            "objc3_runtime_copy_selector_lookup_table_state_for_testing",
            "objc3_runtime_copy_selector_lookup_entry_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "lookup_resolution_order_model": (
            "seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-strict-dispatch-error"
        ),
        "selector_materialization_model": (
            "metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup"
        ),
        "unresolved_selector_behavior_model": (
            "negative-cache-entry-preserved-and-typed-strict-dispatch-error-returned"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_class_metaclass_protocol_realization_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "metaclass-graph-root-class",
        }
    ]
    return {
        "contract_id": RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_realization_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "class_realization_model": (
            "registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection"
        ),
        "metaclass_lineage_model": (
            "realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities"
        ),
        "protocol_conformance_model": (
            "realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/metaclass_graph_root_class_library.objc3",
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/metaclass_graph_root_class_probe.cpp",
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_category_attachment_merged_dispatch_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "canonical-sample-set",
        }
    ]
    return {
        "contract_id": RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.runtime.category.attachment.protocol.conformance.v1",
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_category_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
        ],
        "category_attachment_model": (
            "registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch"
        ),
        "merged_dispatch_resolution_model": (
            "attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-strict-error"
        ),
        "attached_protocol_visibility_model": (
            "attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_reflection_visibility_coherence_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-sample-set",
            "property-reflection-accessor-compatibility-diagnostics",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_coherence_query_boundary": [
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
        ],
        "reflection_visibility_boundary_model": (
            "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state"
        ),
        "fail_closed_lookup_diagnostic_model": (
            "missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state"
        ),
        "runtime_coherence_diagnostic_model": (
            "reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }
