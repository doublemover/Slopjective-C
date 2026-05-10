"""Expected cross-module storage/reflection preservation contracts."""

from __future__ import annotations

from ..runtime_contract_storage_reflection import (
    RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)


DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID = (
    "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
)
EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID = (
    "objc3c.executable.property.accessor.layout.lowering.v1"
)
EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID = (
    "objc3c.executable.ivar.layout.emission.v1"
)
EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID = (
    "objc3c.executable.synthesized.accessor.property.lowering.v1"
)
STORAGE_REFLECTION_PRESERVATION_MODEL = (
    "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-"
    "preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-"
    "ir-object-emission"
)

PROVIDER_STORAGE_FIELDS = {
    "contract_id": RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    "source_contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    "dispatch_and_synthesized_accessor_lowering_surface_contract_id": DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID,
    "executable_property_accessor_layout_lowering_contract_id": EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID,
    "executable_ivar_layout_emission_contract_id": EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID,
    "executable_synthesized_accessor_property_lowering_contract_id": EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID,
    "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_storage_reflection_artifact_preservation",
    "import_artifact_member_name": "objc_runtime_storage_reflection_artifact_preservation",
    "source_model": "runtime-metadata-source-records-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-for-separate-compilation",
    "preservation_model": STORAGE_REFLECTION_PRESERVATION_MODEL,
    "fail_closed_model": "missing-or-drifted-storage-reflection-preservation-packets-disable-cross-module-storage-reflection-claims",
}

STORAGE_REFLECTION_ENTRY_COUNTS = (
    ("implementation_owned_property_entries", 3),
    ("synthesized_accessor_owner_entries", 3),
    ("synthesized_getter_entries", 3),
    ("synthesized_setter_entries", 3),
    ("synthesized_accessor_entries", 6),
    ("current_property_read_entries", 3),
    ("current_property_write_entries", 2),
    ("current_property_exchange_entries", 1),
    ("weak_current_property_load_entries", 0),
    ("weak_current_property_store_entries", 0),
    ("ivar_layout_entries", 3),
    ("ivar_layout_owner_entries", 1),
)

LINK_PLAN_CONTRACT_FIELDS = (
    (
        "runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id",
        RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    ),
    (
        "runtime_property_ivar_storage_accessor_source_surface_contract_id",
        RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    ),
    (
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
        DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID,
    ),
    (
        "executable_property_accessor_layout_lowering_contract_id",
        EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID,
    ),
    (
        "executable_ivar_layout_emission_contract_id",
        EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID,
    ),
    (
        "executable_synthesized_accessor_property_lowering_contract_id",
        EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID,
    ),
    (
        "storage_reflection_artifact_preservation_model",
        STORAGE_REFLECTION_PRESERVATION_MODEL,
    ),
)

IMPORTED_MODULE_FIELDS = (
    ("storage_reflection_artifact_preservation_present", True),
    ("storage_reflection_runtime_import_artifact_ready", True),
    ("storage_reflection_separate_compilation_preservation_ready", True),
    ("storage_reflection_deterministic", True),
    (
        "storage_reflection_contract_id",
        RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    ),
    (
        "storage_reflection_source_contract_id",
        RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    ),
    (
        "storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id",
        DISPATCH_AND_SYNTHESIZED_ACCESSOR_SURFACE_CONTRACT_ID,
    ),
    (
        "storage_reflection_executable_property_accessor_layout_lowering_contract_id",
        EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_CONTRACT_ID,
    ),
    (
        "storage_reflection_executable_ivar_layout_emission_contract_id",
        EXECUTABLE_IVAR_LAYOUT_EMISSION_CONTRACT_ID,
    ),
    (
        "storage_reflection_executable_synthesized_accessor_property_lowering_contract_id",
        EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_CONTRACT_ID,
    ),
    ("storage_reflection_local_property_descriptor_count", 6),
    ("storage_reflection_local_ivar_descriptor_count", 3),
    ("storage_reflection_implementation_owned_property_entries", 3),
    ("storage_reflection_synthesized_accessor_owner_entries", 3),
    ("storage_reflection_synthesized_getter_entries", 3),
    ("storage_reflection_synthesized_setter_entries", 3),
    ("storage_reflection_synthesized_accessor_entries", 6),
    ("storage_reflection_current_property_read_entries", 3),
    ("storage_reflection_current_property_write_entries", 2),
    ("storage_reflection_current_property_exchange_entries", 1),
    ("storage_reflection_weak_current_property_load_entries", 0),
    ("storage_reflection_weak_current_property_store_entries", 0),
    ("storage_reflection_ivar_layout_entries", 3),
    ("storage_reflection_ivar_layout_owner_entries", 1),
)

LOCAL_STORAGE_REFLECTION_COUNTS = {
    "local_storage_reflection_implementation_owned_property_entries": 0,
    "local_storage_reflection_synthesized_accessor_owner_entries": 0,
    "local_storage_reflection_synthesized_getter_entries": 0,
    "local_storage_reflection_synthesized_setter_entries": 0,
    "local_storage_reflection_synthesized_accessor_entries": 0,
    "local_storage_reflection_current_property_read_entries": 0,
    "local_storage_reflection_current_property_write_entries": 0,
    "local_storage_reflection_current_property_exchange_entries": 0,
    "local_storage_reflection_weak_current_property_load_entries": 0,
    "local_storage_reflection_weak_current_property_store_entries": 0,
    "local_storage_reflection_ivar_layout_entries": 0,
    "local_storage_reflection_ivar_layout_owner_entries": 0,
}
IMPORTED_STORAGE_REFLECTION_COUNTS = {
    "imported_storage_reflection_implementation_owned_property_entries": 3,
    "imported_storage_reflection_synthesized_accessor_owner_entries": 3,
    "imported_storage_reflection_synthesized_getter_entries": 3,
    "imported_storage_reflection_synthesized_setter_entries": 3,
    "imported_storage_reflection_synthesized_accessor_entries": 6,
    "imported_storage_reflection_current_property_read_entries": 3,
    "imported_storage_reflection_current_property_write_entries": 2,
    "imported_storage_reflection_current_property_exchange_entries": 1,
    "imported_storage_reflection_weak_current_property_load_entries": 0,
    "imported_storage_reflection_weak_current_property_store_entries": 0,
    "imported_storage_reflection_ivar_layout_entries": 3,
    "imported_storage_reflection_ivar_layout_owner_entries": 1,
}
TRANSITIVE_STORAGE_REFLECTION_COUNTS = {
    "transitive_storage_reflection_implementation_owned_property_entries": 3,
    "transitive_storage_reflection_synthesized_accessor_owner_entries": 3,
    "transitive_storage_reflection_synthesized_getter_entries": 3,
    "transitive_storage_reflection_synthesized_setter_entries": 3,
    "transitive_storage_reflection_synthesized_accessor_entries": 6,
    "transitive_storage_reflection_current_property_read_entries": 3,
    "transitive_storage_reflection_current_property_write_entries": 2,
    "transitive_storage_reflection_current_property_exchange_entries": 1,
    "transitive_storage_reflection_weak_current_property_load_entries": 0,
    "transitive_storage_reflection_weak_current_property_store_entries": 0,
    "transitive_storage_reflection_ivar_layout_entries": 3,
    "transitive_storage_reflection_ivar_layout_owner_entries": 1,
}


__all__ = [
    "IMPORTED_MODULE_FIELDS",
    "IMPORTED_STORAGE_REFLECTION_COUNTS",
    "LINK_PLAN_CONTRACT_FIELDS",
    "LOCAL_STORAGE_REFLECTION_COUNTS",
    "PROVIDER_STORAGE_FIELDS",
    "STORAGE_REFLECTION_ENTRY_COUNTS",
    "TRANSITIVE_STORAGE_REFLECTION_COUNTS",
]
