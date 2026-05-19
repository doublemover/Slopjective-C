"""Core runtime acceptance catalog surfaces."""

from __future__ import annotations

from ..contract_ids import *
from ..model import SurfaceRequirement


FOUNDATION_SURFACES: tuple[SurfaceRequirement, ...] = (
    SurfaceRequirement("claim_boundary", CLAIM_BOUNDARY_CONTRACT_ID),
    SurfaceRequirement(
        "runtime_state_publication_surface",
        RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        ("publication_surface_kind", "compile_artifact_set", "public_runtime_abi_boundary"),
    ),
    SurfaceRequirement(
        "acceptance_suite_surface",
        RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
        (
            "suite_path",
            "report_path",
            "authoritative_claim_classes",
            "compile_output_provenance_contract_id",
            "compile_output_truthfulness_contract_id",
        ),
    ),
)

__all__ = ["FOUNDATION_SURFACES"]
