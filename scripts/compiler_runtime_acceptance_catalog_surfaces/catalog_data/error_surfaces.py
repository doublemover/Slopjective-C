"""Runtime error acceptance catalog surfaces."""

from __future__ import annotations

from ..contract_ids import *
from ..evidence_fields import AUTHORITATIVE_PROBE_PATH_FIELD, SOURCE_CODE_AND_CASE_FIELDS
from ..model import SurfaceRequirement


ERROR_SURFACES: tuple[SurfaceRequirement, ...] = (
    SurfaceRequirement(
        "runtime_error_execution_cleanup_source_surface",
        RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_catch_filter_finalization_source_surface",
        RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_error_propagation_cleanup_semantics_surface",
        RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_bridging_filter_unwind_diagnostics_surface",
        RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_error_lowering_unwind_bridge_helper_surface",
        RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
        SOURCE_CODE_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_error_runtime_abi_cleanup_surface",
        RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
        ("authoritative_case_ids", AUTHORITATIVE_PROBE_PATH_FIELD, "public_runtime_abi_boundary"),
    ),
    SurfaceRequirement(
        "runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
        RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        (
            "authoritative_case_ids",
            "authoritative_probe_paths",
            "private_error_runtime_abi_boundary",
        ),
    ),
)

__all__ = ["ERROR_SURFACES"]
