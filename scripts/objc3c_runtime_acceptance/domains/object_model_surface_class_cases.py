"""Object Model class, category, and diagnostic acceptance surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..c_api import PUBLIC_RUNTIME_ABI_BOUNDARY
from ..runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
)
from ..runtime_contract_object_model import (
    RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_class_metaclass_protocol_realization_surface",
    "build_runtime_category_attachment_merged_dispatch_surface",
    "build_runtime_reflection_visibility_coherence_diagnostics_surface",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


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


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
