"""Object Model realization lookup semantics surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.object_model_surface_query_support import (
    authoritative_case_ids,
)

from ..c_api import PUBLIC_RUNTIME_ABI_BOUNDARY
from ..runtime_contract_object_model import (
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
)


def build_runtime_realization_lookup_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "canonical-dispatch",
                "canonical-sample-set",
                "dispatch-fast-path",
            },
        ),
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


__all__ = ["build_runtime_realization_lookup_semantics_surface"]
