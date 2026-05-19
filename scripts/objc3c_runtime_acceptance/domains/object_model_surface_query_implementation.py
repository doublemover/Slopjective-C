"""Object Model lookup/reflection runtime implementation surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.object_model_surface_query_support import (
    authoritative_case_ids,
)

from ..c_api import RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH
from ..runtime_contract_object_model import (
    REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
    RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
)


def build_runtime_realization_lookup_reflection_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "realization-lookup-reflection-runtime",
                "canonical-sample-set",
                "dispatch-fast-path",
            },
        ),
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


__all__ = ["build_runtime_realization_lookup_reflection_implementation_surface"]
