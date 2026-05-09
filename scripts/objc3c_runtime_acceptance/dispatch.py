"""Dispatch and property-accessor runtime acceptance surfaces."""

from __future__ import annotations

from objc3c_runtime_acceptance.runtime_contracts import (
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)


def build_dispatch_accessor_runtime_abi_surface() -> dict[str, object]:
    return {
        "contract_id": "objc3c.runtime.dispatch_accessor.abi.surface.v1",
        "proof_cases": [
            "canonical-sample-set",
            "dispatch-fast-path",
            "synthesized-accessor-runtime",
            "property-layout",
            "instance-allocation-layout-runtime",
            "property-execution",
            "arc-property-helper-abi",
        ],
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
        "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
        "property_registry_state_snapshot_symbol": "objc3_runtime_copy_property_registry_state_for_testing",
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "arc_debug_state_snapshot_symbol": "objc3_runtime_copy_arc_debug_state_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
        "weak_current_property_load_symbol": "objc3_runtime_load_weak_current_property_i32",
        "weak_current_property_store_symbol": "objc3_runtime_store_weak_current_property_i32",
        "retain_symbol": "objc3_runtime_retain_i32",
        "release_symbol": "objc3_runtime_release_i32",
        "autorelease_symbol": "objc3_runtime_autorelease_i32",
        "private_testing_surface_only": True,
        "deterministic": True,
    }


def build_storage_accessor_runtime_abi_surface() -> dict[str, object]:
    return {
        "contract_id": RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "proof_cases": [
            "dispatch-fast-path",
            "accessor-storage-lowering-metadata-surface",
            "property-accessor-layout-lowering",
            "synthesized-accessor-runtime",
            "property-layout",
            "instance-allocation-layout-runtime",
            "property-execution",
            "arc-property-helper-abi",
        ],
        "property_registry_state_snapshot_symbol": "objc3_runtime_copy_property_registry_state_for_testing",
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": "objc3_runtime_load_weak_current_property_i32",
        "weak_current_property_store_symbol": "objc3_runtime_store_weak_current_property_i32",
        "private_testing_surface_only": True,
        "deterministic": True,
    }


def build_property_ivar_accessor_reflection_implementation_surface() -> dict[str, object]:
    return {
        "contract_id": (
            RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "proof_cases": [
            "property-execution",
            "property-layout",
            "instance-allocation-layout-runtime",
            "storage-ownership-reflection",
        ],
        "implementation_snapshot_symbol": (
            "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
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
        "deterministic": True,
    }


__all__ = [
    "build_dispatch_accessor_runtime_abi_surface",
    "build_property_ivar_accessor_reflection_implementation_surface",
    "build_storage_accessor_runtime_abi_surface",
]
