"""Storage ownership reflection assertion catalog."""

from __future__ import annotations

from objc3c_runtime_acceptance.c_api import RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH
from objc3c_runtime_acceptance.c_api import RUNTIME_PUBLIC_HEADER_PATH
from objc3c_runtime_acceptance.runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)

from .data import LlTextExpectation
from .data import SurfaceFieldExpectation


MANIFEST_IMPLEMENTATION_SURFACE_KEY = (
    "runtime_property_ivar_accessor_reflection_implementation_surface"
)

BOX_REALIZED_MESSAGE = "expected Box to be realized for storage ownership reflection"
BOX_ACCESSOR_COUNT_MESSAGE = (
    "expected Box to publish five runtime-backed storage accessors"
)
BOX_INSTANCE_SIZE_MESSAGE = (
    "expected Box instance layout to reserve five object-backed storage slots"
)

REGISTRATION_MANIFEST_EXPECTATIONS = (
    SurfaceFieldExpectation(
        "property_descriptor_count",
        10,
        "expected storage ownership fixture to publish ten property descriptors",
    ),
    SurfaceFieldExpectation(
        "ivar_descriptor_count",
        5,
        "expected storage ownership fixture to publish five ivar layout descriptors",
    ),
    SurfaceFieldExpectation(
        "compile_output_truthfulness_property_descriptor_count",
        10,
        "expected compile-output truthfulness to certify ten property descriptors",
    ),
    SurfaceFieldExpectation(
        "compile_output_truthfulness_ivar_descriptor_count",
        5,
        "expected compile-output truthfulness to certify five ivar descriptors",
    ),
)

COMPILE_OWNERSHIP_LL_EXPECTATIONS = (
    LlTextExpectation(
        "; runtime_backed_object_ownership_attribute_surface = "
        "contract=objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        "expected LLVM IR to publish the runtime-backed object ownership attribute surface",
    ),
    LlTextExpectation(
        "property_attribute_profiles=10",
        "expected LLVM IR ownership surface to publish ten property-attribute profiles",
    ),
    LlTextExpectation(
        "ownership_lifetime_profiles=10",
        "expected LLVM IR ownership surface to publish ten ownership lifetime profiles",
    ),
    LlTextExpectation(
        "ownership_runtime_hook_profiles=6",
        "expected LLVM IR ownership surface to publish six runtime hook profiles",
    ),
    LlTextExpectation(
        "accessor_ownership_profiles=10",
        "expected LLVM IR ownership surface to publish ten accessor ownership profiles",
    ),
)

MANIFEST_IMPLEMENTATION_SURFACE_EXPECTATIONS = (
    SurfaceFieldExpectation(
        "contract_id",
        RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected property/accessor runtime implementation surface to preserve contract_id",
    ),
    SurfaceFieldExpectation(
        "runtime_property_ivar_storage_accessor_source_surface_contract_id",
        RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected property/accessor runtime implementation surface to preserve runtime_property_ivar_storage_accessor_source_surface_contract_id",
    ),
    SurfaceFieldExpectation(
        "storage_accessor_runtime_abi_surface_contract_id",
        RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "expected property/accessor runtime implementation surface to preserve storage_accessor_runtime_abi_surface_contract_id",
    ),
    SurfaceFieldExpectation(
        "property_metadata_reflection_contract_id",
        "objc3c.runtime.property.metadata.reflection.v1",
        "expected property/accessor runtime implementation surface to preserve property_metadata_reflection_contract_id",
    ),
    SurfaceFieldExpectation(
        "runtime_backed_object_ownership_attribute_surface_contract_id",
        "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        "expected property/accessor runtime implementation surface to preserve runtime_backed_object_ownership_attribute_surface_contract_id",
    ),
    SurfaceFieldExpectation(
        "public_header_path",
        RUNTIME_PUBLIC_HEADER_PATH,
        "expected property/accessor runtime implementation surface to preserve public_header_path",
    ),
    SurfaceFieldExpectation(
        "internal_header_path",
        RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "expected property/accessor runtime implementation surface to preserve internal_header_path",
    ),
    SurfaceFieldExpectation(
        "implementation_snapshot_symbol",
        "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing",
        "expected property/accessor runtime implementation surface to preserve implementation_snapshot_symbol",
    ),
    SurfaceFieldExpectation(
        "property_registry_state_snapshot_symbol",
        "objc3_runtime_copy_property_registry_state_for_testing",
        "expected property/accessor runtime implementation surface to preserve property_registry_state_snapshot_symbol",
    ),
    SurfaceFieldExpectation(
        "property_entry_snapshot_symbol",
        "objc3_runtime_copy_property_entry_for_testing",
        "expected property/accessor runtime implementation surface to preserve property_entry_snapshot_symbol",
    ),
    SurfaceFieldExpectation(
        "current_property_read_symbol",
        "objc3_runtime_read_current_property_i32",
        "expected property/accessor runtime implementation surface to preserve current_property_read_symbol",
    ),
    SurfaceFieldExpectation(
        "current_property_write_symbol",
        "objc3_runtime_write_current_property_i32",
        "expected property/accessor runtime implementation surface to preserve current_property_write_symbol",
    ),
    SurfaceFieldExpectation(
        "current_property_exchange_symbol",
        "objc3_runtime_exchange_current_property_i32",
        "expected property/accessor runtime implementation surface to preserve current_property_exchange_symbol",
    ),
    SurfaceFieldExpectation(
        "bind_current_property_context_symbol",
        "objc3_runtime_bind_current_property_context_for_testing",
        "expected property/accessor runtime implementation surface to preserve bind_current_property_context_symbol",
    ),
    SurfaceFieldExpectation(
        "clear_current_property_context_symbol",
        "objc3_runtime_clear_current_property_context_for_testing",
        "expected property/accessor runtime implementation surface to preserve clear_current_property_context_symbol",
    ),
    SurfaceFieldExpectation(
        "weak_current_property_load_symbol",
        "objc3_runtime_load_weak_current_property_i32",
        "expected property/accessor runtime implementation surface to preserve weak_current_property_load_symbol",
    ),
    SurfaceFieldExpectation(
        "weak_current_property_store_symbol",
        "objc3_runtime_store_weak_current_property_i32",
        "expected property/accessor runtime implementation surface to preserve weak_current_property_store_symbol",
    ),
    SurfaceFieldExpectation(
        "implementation_model",
        "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-through-runtime-storage-owners",
        "expected property/accessor runtime implementation surface to preserve implementation_model",
    ),
    SurfaceFieldExpectation(
        "reflection_model",
        "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts",
        "expected property/accessor runtime implementation surface to preserve reflection_model",
    ),
    SurfaceFieldExpectation(
        "fail_closed_model",
        "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-synthetic-storage-path",
        "expected property/accessor runtime implementation surface to preserve fail_closed_model",
    ),
)

MANIFEST_IMPLEMENTATION_REQUIREMENTS_EXPECTATIONS = (
    SurfaceFieldExpectation(
        "requires_coupled_registration_manifest",
        True,
        "expected property/accessor runtime implementation surface to require the coupled runtime registration manifest",
    ),
    SurfaceFieldExpectation(
        "requires_real_compile_output",
        True,
        "expected property/accessor runtime implementation surface to require real compile output",
    ),
    SurfaceFieldExpectation(
        "requires_linked_runtime_probe",
        True,
        "expected property/accessor runtime implementation surface to require a linked runtime probe",
    ),
)

LIVE_IMPLEMENTATION_SURFACE_EXPECTATIONS = (
    SurfaceFieldExpectation(
        "property_registry_ready",
        1,
        "expected live storage/accessor implementation snapshot to preserve property_registry_ready",
    ),
    SurfaceFieldExpectation(
        "runtime_accessor_dispatch_ready",
        1,
        "expected live storage/accessor implementation snapshot to preserve runtime_accessor_dispatch_ready",
    ),
    SurfaceFieldExpectation(
        "runtime_layout_ready",
        1,
        "expected live storage/accessor implementation snapshot to preserve runtime_layout_ready",
    ),
    SurfaceFieldExpectation(
        "reflection_query_ready",
        1,
        "expected live storage/accessor implementation snapshot to preserve reflection_query_ready",
    ),
    SurfaceFieldExpectation(
        "deterministic",
        1,
        "expected live storage/accessor implementation snapshot to preserve deterministic",
    ),
    SurfaceFieldExpectation(
        "property_registry_state_snapshot_symbol",
        "objc3_runtime_copy_property_registry_state_for_testing",
        "expected live storage/accessor implementation snapshot to preserve property_registry_state_snapshot_symbol",
    ),
    SurfaceFieldExpectation(
        "property_entry_snapshot_symbol",
        "objc3_runtime_copy_property_entry_for_testing",
        "expected live storage/accessor implementation snapshot to preserve property_entry_snapshot_symbol",
    ),
    SurfaceFieldExpectation(
        "current_property_read_symbol",
        "objc3_runtime_read_current_property_i32",
        "expected live storage/accessor implementation snapshot to preserve current_property_read_symbol",
    ),
    SurfaceFieldExpectation(
        "current_property_write_symbol",
        "objc3_runtime_write_current_property_i32",
        "expected live storage/accessor implementation snapshot to preserve current_property_write_symbol",
    ),
    SurfaceFieldExpectation(
        "current_property_exchange_symbol",
        "objc3_runtime_exchange_current_property_i32",
        "expected live storage/accessor implementation snapshot to preserve current_property_exchange_symbol",
    ),
    SurfaceFieldExpectation(
        "bind_current_property_context_symbol",
        "objc3_runtime_bind_current_property_context_for_testing",
        "expected live storage/accessor implementation snapshot to preserve bind_current_property_context_symbol",
    ),
    SurfaceFieldExpectation(
        "clear_current_property_context_symbol",
        "objc3_runtime_clear_current_property_context_for_testing",
        "expected live storage/accessor implementation snapshot to preserve clear_current_property_context_symbol",
    ),
    SurfaceFieldExpectation(
        "weak_current_property_load_symbol",
        "objc3_runtime_load_weak_current_property_i32",
        "expected live storage/accessor implementation snapshot to preserve weak_current_property_load_symbol",
    ),
    SurfaceFieldExpectation(
        "weak_current_property_store_symbol",
        "objc3_runtime_store_weak_current_property_i32",
        "expected live storage/accessor implementation snapshot to preserve weak_current_property_store_symbol",
    ),
    SurfaceFieldExpectation(
        "implementation_model",
        "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-through-runtime-storage-owners",
        "expected live storage/accessor implementation snapshot to preserve implementation_model",
    ),
    SurfaceFieldExpectation(
        "reflection_model",
        "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts",
        "expected live storage/accessor implementation snapshot to preserve reflection_model",
    ),
    SurfaceFieldExpectation(
        "fail_closed_model",
        "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-synthetic-storage-path",
        "expected live storage/accessor implementation snapshot to preserve fail_closed_model",
    ),
)

__all__ = [
    "BOX_ACCESSOR_COUNT_MESSAGE",
    "BOX_INSTANCE_SIZE_MESSAGE",
    "BOX_REALIZED_MESSAGE",
    "COMPILE_OWNERSHIP_LL_EXPECTATIONS",
    "LIVE_IMPLEMENTATION_SURFACE_EXPECTATIONS",
    "MANIFEST_IMPLEMENTATION_REQUIREMENTS_EXPECTATIONS",
    "MANIFEST_IMPLEMENTATION_SURFACE_EXPECTATIONS",
    "MANIFEST_IMPLEMENTATION_SURFACE_KEY",
    "REGISTRATION_MANIFEST_EXPECTATIONS",
]
