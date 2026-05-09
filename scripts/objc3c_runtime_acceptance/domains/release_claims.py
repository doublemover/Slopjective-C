"""Release Claims runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.release_claims_publication_cases import (
    check_claim_publication_dashboard_schema_surface_case,
    check_final_claim_publication_deprecated_path_shutdown_case,
    check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case,
)
from objc3c_runtime_acceptance.domains.release_claims_runtime_cases import (
    check_final_release_evidence_descaffolding_implementation_case,
    check_release_candidate_runtime_claim_abi_case,
)
from objc3c_runtime_acceptance.domains.release_claims_source_cases import (
    check_claimability_semantics_release_policy_case,
    check_claimable_surface_residual_non_claimable_gaps_source_surface_case,
    check_strict_profile_claim_implementation_case,
    check_strict_profile_feature_claim_source_surface_case,
)
from objc3c_runtime_acceptance.domains.release_claims_surfaces import (
    build_runtime_claim_publication_dashboard_schema_surface,
    build_runtime_claimability_semantics_release_policy_surface,
    build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface,
    build_runtime_final_claim_publication_deprecated_path_shutdown_surface,
    build_runtime_final_release_evidence_descaffolding_implementation_surface,
    build_runtime_release_candidate_claim_abi_surface,
    build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface,
    build_runtime_strict_profile_claim_implementation_surface,
    build_runtime_strict_profile_feature_claim_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
    "build_runtime_strict_profile_feature_claim_source_surface",
    "build_runtime_claimability_semantics_release_policy_surface",
    "build_runtime_strict_profile_claim_implementation_surface",
    "build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface",
    "build_runtime_claim_publication_dashboard_schema_surface",
    "build_runtime_final_claim_publication_deprecated_path_shutdown_surface",
    "build_runtime_release_candidate_claim_abi_surface",
    "build_runtime_final_release_evidence_descaffolding_implementation_surface",
    "check_claimable_surface_residual_non_claimable_gaps_source_surface_case",
    "check_strict_profile_feature_claim_source_surface_case",
    "check_claimability_semantics_release_policy_case",
    "check_strict_profile_claim_implementation_case",
    "check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case",
    "check_claim_publication_dashboard_schema_surface_case",
    "check_final_claim_publication_deprecated_path_shutdown_case",
    "check_release_candidate_runtime_claim_abi_case",
    "check_final_release_evidence_descaffolding_implementation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
