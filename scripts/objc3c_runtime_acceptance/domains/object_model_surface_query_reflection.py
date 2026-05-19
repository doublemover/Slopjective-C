"""Object Model reflection query surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.object_model_surface_query_support import (
    authoritative_case_ids,
)

from ..c_api import PUBLIC_RUNTIME_ABI_BOUNDARY
from ..runtime_contract_object_model import (
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
)


def build_runtime_reflection_query_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "canonical-sample-set",
                "property-reflection",
                "storage-ownership-reflection",
            },
        ),
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


__all__ = ["build_runtime_reflection_query_surface"]
