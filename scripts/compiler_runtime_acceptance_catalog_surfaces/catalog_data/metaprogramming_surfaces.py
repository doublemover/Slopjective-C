"""Metaprogramming acceptance catalog surfaces."""

from __future__ import annotations

from ..contract_ids import *
from ..evidence_fields import SOURCE_CODE_AND_CASE_FIELDS, case_evidence_fields
from ..model import SurfaceRequirement


METAPROGRAMMING_SURFACES: tuple[SurfaceRequirement, ...] = (
    SurfaceRequirement(
        "runtime_metaprogramming_source_surface",
        RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_package_provenance_source_surface",
        RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_semantics_surface",
        RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
        case_evidence_fields("semantic_contract_ids", "authoritative_code_paths"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_lowering_host_cache_surface",
        RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
        case_evidence_fields("host_cache_artifact", "expansion_lowering_contract_id"),
    ),
    SurfaceRequirement(
        "runtime_cross_module_metaprogramming_artifact_preservation_surface",
        RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        case_evidence_fields("runtime_import_surface_artifact", "cross_module_link_plan_artifact"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_runtime_abi_cache_surface",
        RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
        (
            "macro_host_process_cache_integration_snapshot_symbol",
            "authoritative_probe_paths",
            "authoritative_case_ids",
        ),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_cache_runtime_integration_implementation_surface",
        RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        (
            "authoritative_probe_paths",
            "authoritative_case_ids",
            "macro_host_process_cache_integration_snapshot_symbol",
        ),
    ),
)

__all__ = ["METAPROGRAMMING_SURFACES"]
