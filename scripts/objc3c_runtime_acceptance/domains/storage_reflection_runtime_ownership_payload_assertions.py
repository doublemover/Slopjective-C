"""Runtime ownership payload and compile-surface assertions."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_sources import (
    StorageOwnershipReflectionArtifacts,
)

from ..c_api import RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH
from ..c_api import RUNTIME_PUBLIC_HEADER_PATH
from ..runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)


@dataclass(frozen=True)
class StorageOwnershipReflectionFacts:
    payload: dict[str, Any]
    box_entry: dict[str, Any]
    implementation_surface: dict[str, Any]
    manifest_implementation_surface: dict[str, Any]


def capture_storage_ownership_reflection_facts(
    payload: dict[str, Any],
    artifacts: StorageOwnershipReflectionArtifacts,
) -> StorageOwnershipReflectionFacts:
    return StorageOwnershipReflectionFacts(
        payload=payload,
        box_entry=payload.get("box_entry", {}),
        implementation_surface=payload.get("implementation_surface", {}),
        manifest_implementation_surface=artifacts.manifest.get(
            "runtime_property_ivar_accessor_reflection_implementation_surface",
            {},
        ),
    )


def assert_storage_ownership_runtime_payload(
    facts: StorageOwnershipReflectionFacts,
    artifacts: StorageOwnershipReflectionArtifacts,
) -> None:
    _assert_box_reflection_payload(facts)
    _assert_registration_manifest(artifacts)
    _assert_compile_ownership_surface(artifacts)
    _assert_manifest_implementation_surface(facts)
    _assert_live_implementation_surface(facts)


def _assert_box_reflection_payload(facts: StorageOwnershipReflectionFacts) -> None:
    box_entry = facts.box_entry
    expect(
        box_entry.get("found") == 1,
        "expected Box to be realized for storage ownership reflection",
    )
    expect(
        box_entry.get("runtime_property_accessor_count", 0) >= 5,
        "expected Box to publish five runtime-backed storage accessors",
    )
    expect(
        box_entry.get("runtime_instance_size_bytes", 0) >= 40,
        "expected Box instance layout to reserve five object-backed storage slots",
    )


def _assert_registration_manifest(
    artifacts: StorageOwnershipReflectionArtifacts,
) -> None:
    registration_manifest = artifacts.registration_manifest
    expect(
        registration_manifest.get("property_descriptor_count") == 10,
        "expected storage ownership fixture to publish ten property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 5,
        "expected storage ownership fixture to publish five ivar layout descriptors",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_property_descriptor_count")
        == 10,
        "expected compile-output truthfulness to certify ten property descriptors",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count")
        == 5,
        "expected compile-output truthfulness to certify five ivar descriptors",
    )


def _assert_compile_ownership_surface(
    artifacts: StorageOwnershipReflectionArtifacts,
) -> None:
    expect(
        "; runtime_backed_object_ownership_attribute_surface = "
        "contract=objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        in artifacts.ll_text,
        "expected LLVM IR to publish the runtime-backed object ownership attribute surface",
    )
    expect(
        "property_attribute_profiles=10" in artifacts.ll_text,
        "expected LLVM IR ownership surface to publish ten property-attribute profiles",
    )
    expect(
        "ownership_lifetime_profiles=10" in artifacts.ll_text,
        "expected LLVM IR ownership surface to publish ten ownership lifetime profiles",
    )
    expect(
        "ownership_runtime_hook_profiles=6" in artifacts.ll_text,
        "expected LLVM IR ownership surface to publish six runtime hook profiles",
    )
    expect(
        "accessor_ownership_profiles=10" in artifacts.ll_text,
        "expected LLVM IR ownership surface to publish ten accessor ownership profiles",
    )


def _assert_manifest_implementation_surface(
    facts: StorageOwnershipReflectionFacts,
) -> None:
    manifest_surface = facts.manifest_implementation_surface
    for field, expected_value in _expected_manifest_implementation_surface().items():
        expect(
            manifest_surface.get(field) == expected_value,
            f"expected property/accessor runtime implementation surface to preserve {field}",
        )
    expect(
        manifest_surface.get("requires_coupled_registration_manifest") is True,
        "expected property/accessor runtime implementation surface to require the coupled runtime registration manifest",
    )
    expect(
        manifest_surface.get("requires_real_compile_output") is True,
        "expected property/accessor runtime implementation surface to require real compile output",
    )
    expect(
        manifest_surface.get("requires_linked_runtime_probe") is True,
        "expected property/accessor runtime implementation surface to require a linked runtime probe",
    )


def _assert_live_implementation_surface(
    facts: StorageOwnershipReflectionFacts,
) -> None:
    implementation_surface = facts.implementation_surface
    for field, expected_value in _expected_live_implementation_surface().items():
        expect(
            implementation_surface.get(field) == expected_value,
            f"expected live storage/accessor implementation snapshot to preserve {field}",
        )


def _expected_manifest_implementation_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "storage_accessor_runtime_abi_surface_contract_id": (
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "implementation_snapshot_symbol": (
            "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
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
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-retired-route-synthesis"
        ),
    }


def _expected_live_implementation_surface() -> dict[str, Any]:
    return {
        "property_registry_ready": 1,
        "runtime_accessor_dispatch_ready": 1,
        "runtime_layout_ready": 1,
        "reflection_query_ready": 1,
        "deterministic": 1,
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
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
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-retired-route-synthesis"
        ),
    }


__all__ = [
    "StorageOwnershipReflectionFacts",
    "assert_storage_ownership_runtime_payload",
    "capture_storage_ownership_reflection_facts",
]
