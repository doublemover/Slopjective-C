"""Object model, block ARC, and storage acceptance catalog surfaces."""

from __future__ import annotations

from ..contract_ids import *
from ..evidence_fields import SOURCE_CODE_AND_CASE_FIELDS, case_evidence_fields, source_model_case_fields
from ..model import SurfaceRequirement


OBJECT_STORAGE_SURFACES: tuple[SurfaceRequirement, ...] = (
    SurfaceRequirement(
        "runtime_object_model_realization_source_surface",
        RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        source_model_case_fields("private_object_model_query_boundary"),
    ),
    SurfaceRequirement(
        "runtime_block_arc_unified_source_surface",
        RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_ownership_transfer_capture_family_source_surface",
        RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
        case_evidence_fields("block_capture_ownership_contract_id", "authoritative_code_paths"),
    ),
    SurfaceRequirement(
        "runtime_block_arc_lowering_helper_surface",
        RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        case_evidence_fields("runtime_block_arc_runtime_abi_surface_contract_id", "semantic_surface_paths"),
    ),
    SurfaceRequirement(
        "runtime_block_arc_runtime_abi_surface",
        RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        case_evidence_fields("block_arc_runtime_abi_snapshot_symbol", "arc_debug_state_snapshot_symbol"),
    ),
    SurfaceRequirement(
        "runtime_property_ivar_storage_accessor_source_surface",
        RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "storage_accessor_runtime_abi_surface",
        RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        ("property_registry_state_snapshot_symbol", "current_property_read_symbol", "weak_current_property_store_symbol"),
    ),
    SurfaceRequirement(
        "runtime_property_ivar_accessor_reflection_implementation_surface",
        RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        ("implementation_snapshot_symbol", "property_registry_state_snapshot_symbol", "property_entry_snapshot_symbol"),
    ),
)

__all__ = ["OBJECT_STORAGE_SURFACES"]
