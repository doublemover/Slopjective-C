"""Object Model ABI query surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.object_model_surface_query_support import (
    authoritative_case_ids,
)

from ..c_api import (
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from ..runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
)
from ..runtime_contract_object_model import (
    REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
    RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
)
from ..runtime_contract_registration import MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE


def build_runtime_object_model_abi_query_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
            "objc3_runtime_copy_instance_entry_for_testing",
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
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
            },
        ),
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


__all__ = ["build_runtime_object_model_abi_query_surface"]
