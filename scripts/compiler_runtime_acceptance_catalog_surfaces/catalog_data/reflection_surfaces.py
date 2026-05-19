"""Reflection and realization acceptance catalog surfaces."""

from __future__ import annotations

from ..contract_ids import *
from ..evidence_fields import SOURCE_CODE_AND_CASE_FIELDS, case_evidence_fields, source_model_case_fields
from ..model import SurfaceRequirement


REFLECTION_SURFACES: tuple[SurfaceRequirement, ...] = (
    SurfaceRequirement(
        "runtime_property_atomicity_synthesis_reflection_source_surface",
        RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_realization_lowering_reflection_artifact_surface",
        RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        source_model_case_fields("lowering_artifact_boundary_model"),
    ),
    SurfaceRequirement(
        "runtime_dispatch_table_reflection_record_lowering_surface",
        RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        source_model_case_fields("dispatch_table_lowering_model"),
    ),
    SurfaceRequirement(
        "runtime_cross_module_realized_metadata_replay_preservation_surface",
        RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        source_model_case_fields("realized_metadata_replay_preservation_model"),
    ),
    SurfaceRequirement(
        "runtime_object_model_abi_query_surface",
        RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
        source_model_case_fields("private_object_model_query_boundary"),
    ),
    SurfaceRequirement(
        "runtime_realization_lookup_reflection_implementation_surface",
        RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        source_model_case_fields("object_model_query_state_snapshot_symbol"),
    ),
    SurfaceRequirement(
        "runtime_reflection_query_surface",
        RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
        case_evidence_fields("query_api_boundary_model", "private_query_symbols"),
    ),
    SurfaceRequirement(
        "runtime_realization_lookup_semantics_surface",
        RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        case_evidence_fields("private_lookup_query_boundary", "lookup_resolution_order_model"),
    ),
    SurfaceRequirement(
        "runtime_class_metaclass_protocol_realization_surface",
        RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        case_evidence_fields("private_realization_query_boundary", "metaclass_lineage_model"),
    ),
    SurfaceRequirement(
        "runtime_category_attachment_merged_dispatch_surface",
        RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
        case_evidence_fields("private_category_query_boundary", "merged_dispatch_resolution_model"),
    ),
    SurfaceRequirement(
        "runtime_reflection_visibility_coherence_diagnostics_surface",
        RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        case_evidence_fields("private_coherence_query_boundary", "runtime_coherence_diagnostic_model"),
    ),
)

__all__ = ["REFLECTION_SURFACES"]
