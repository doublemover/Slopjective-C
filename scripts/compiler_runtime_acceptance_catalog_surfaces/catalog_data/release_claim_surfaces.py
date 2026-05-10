"""Release-claim acceptance catalog surfaces."""

from __future__ import annotations

from ..contract_ids import *
from ..evidence_fields import SOURCE_FIELD_AND_CASE_FIELDS, probe_path_fields, source_model_case_fields
from ..model import SurfaceRequirement


RELEASE_CLAIM_SURFACES: tuple[SurfaceRequirement, ...] = (
    SurfaceRequirement(
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
        RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_FIELD_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_strict_profile_feature_claim_source_surface",
        RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
        SOURCE_FIELD_AND_CASE_FIELDS,
    ),
    SurfaceRequirement(
        "runtime_claimability_semantics_release_policy_surface",
        RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
        source_model_case_fields("policy_model"),
    ),
    SurfaceRequirement(
        "runtime_strict_profile_claim_implementation_surface",
        RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        source_model_case_fields("claim_implementation_model"),
    ),
    SurfaceRequirement(
        "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface",
        RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        source_model_case_fields("compatibility_diagnostic_model"),
    ),
    SurfaceRequirement(
        "runtime_claim_publication_dashboard_schema_surface",
        RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
        source_model_case_fields("dashboard_schema_path"),
    ),
    SurfaceRequirement(
        "runtime_final_claim_publication_deprecated_path_shutdown_surface",
        RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
        source_model_case_fields("final_publication_model"),
    ),
    SurfaceRequirement(
        "runtime_release_candidate_claim_abi_surface",
        RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        probe_path_fields(
            "release_candidate_claim_snapshot_symbol",
            "release_candidate_claim_snapshot_type",
        ),
    ),
    SurfaceRequirement(
        "runtime_final_release_evidence_descaffolding_implementation_surface",
        RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        probe_path_fields("release_candidate_evidence_snapshot_symbol", "implementation_model"),
    ),
)

__all__ = ["RELEASE_CLAIM_SURFACES"]
