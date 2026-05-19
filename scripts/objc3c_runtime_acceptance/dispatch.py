"""Dispatch and property-accessor runtime acceptance surfaces."""

from __future__ import annotations

from objc3c_runtime_acceptance.runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.dispatch_accessor_symbols import (
    DISPATCH_ACCESSOR_PROOF_CASES,
    DISPATCH_ACCESSOR_SYMBOLS,
    PROPERTY_IVAR_REFLECTION_PROOF_CASES,
    PROPERTY_IVAR_REFLECTION_SYMBOLS,
    STORAGE_ACCESSOR_PROOF_CASES,
    STORAGE_ACCESSOR_SYMBOLS,
    private_deterministic_symbol_surface,
)


def build_dispatch_accessor_runtime_abi_surface() -> dict[str, object]:
    return private_deterministic_symbol_surface(
        contract_id="objc3c.runtime.dispatch_accessor.abi.surface.v1",
        proof_cases=DISPATCH_ACCESSOR_PROOF_CASES,
        symbols=DISPATCH_ACCESSOR_SYMBOLS,
    )


def build_storage_accessor_runtime_abi_surface() -> dict[str, object]:
    return private_deterministic_symbol_surface(
        contract_id=RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        proof_cases=STORAGE_ACCESSOR_PROOF_CASES,
        symbols=STORAGE_ACCESSOR_SYMBOLS,
    )


def build_property_ivar_accessor_reflection_implementation_surface() -> dict[str, object]:
    return private_deterministic_symbol_surface(
        contract_id=(
            RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        proof_cases=PROPERTY_IVAR_REFLECTION_PROOF_CASES,
        symbols=PROPERTY_IVAR_REFLECTION_SYMBOLS,
        extra_fields={
            "implementation_model": (
                "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-through-runtime-storage-owners"
            ),
            "reflection_model": (
                "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
            ),
            "fail_closed_model": (
                "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-synthetic-storage-path"
            ),
        },
        private_testing_surface_only=False,
    )


__all__ = [
    "build_dispatch_accessor_runtime_abi_surface",
    "build_property_ivar_accessor_reflection_implementation_surface",
    "build_storage_accessor_runtime_abi_surface",
]
