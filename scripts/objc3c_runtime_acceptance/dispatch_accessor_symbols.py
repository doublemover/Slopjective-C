"""Dispatch and property-accessor symbol surfaces."""

from __future__ import annotations


DISPATCH_ACCESSOR_PROOF_CASES = [
    "canonical-sample-set",
    "dispatch-fast-path",
    "synthesized-accessor-runtime",
    "property-layout",
    "instance-allocation-layout-runtime",
    "property-execution",
    "arc-property-helper-abi",
]

STORAGE_ACCESSOR_PROOF_CASES = [
    "dispatch-fast-path",
    "accessor-storage-lowering-metadata-surface",
    "property-accessor-layout-lowering",
    "synthesized-accessor-runtime",
    "property-layout",
    "instance-allocation-layout-runtime",
    "property-execution",
    "arc-property-helper-abi",
]

PROPERTY_IVAR_REFLECTION_PROOF_CASES = [
    "property-execution",
    "property-layout",
    "instance-allocation-layout-runtime",
    "storage-ownership-reflection",
]

DISPATCH_ACCESSOR_SYMBOLS = {
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
}

STORAGE_ACCESSOR_SYMBOLS = {
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
}

PROPERTY_IVAR_REFLECTION_SYMBOLS = {
    "implementation_snapshot_symbol": (
        "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing"
    ),
    **STORAGE_ACCESSOR_SYMBOLS,
}


def private_deterministic_symbol_surface(
    *,
    contract_id: str,
    proof_cases: list[str],
    symbols: dict[str, object],
    extra_fields: dict[str, object] | None = None,
    private_testing_surface_only: bool = True,
) -> dict[str, object]:
    payload = {
        "contract_id": contract_id,
        "proof_cases": proof_cases,
        **symbols,
        **(extra_fields or {}),
    }
    if private_testing_surface_only:
        payload["private_testing_surface_only"] = True
    payload["deterministic"] = True
    return payload


__all__ = [
    "DISPATCH_ACCESSOR_PROOF_CASES",
    "DISPATCH_ACCESSOR_SYMBOLS",
    "PROPERTY_IVAR_REFLECTION_PROOF_CASES",
    "PROPERTY_IVAR_REFLECTION_SYMBOLS",
    "STORAGE_ACCESSOR_PROOF_CASES",
    "STORAGE_ACCESSOR_SYMBOLS",
    "private_deterministic_symbol_surface",
]
