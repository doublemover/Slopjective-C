"""Release-candidate live surface construction."""

from __future__ import annotations

from typing import Any

from .command_execution import CaseResult, runtime_acceptance


def build_live_surfaces(
    results: list[CaseResult],
    runtime_backend: Any = runtime_acceptance,
) -> dict[str, dict[str, Any]]:
    return {
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
            runtime_backend.build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface(
                results
            )
        ),
        "runtime_strict_profile_feature_claim_source_surface": (
            runtime_backend.build_runtime_strict_profile_feature_claim_source_surface(
                results
            )
        ),
        "runtime_claimability_semantics_release_policy_surface": (
            runtime_backend.build_runtime_claimability_semantics_release_policy_surface(
                results
            )
        ),
        "runtime_strict_profile_claim_implementation_surface": (
            runtime_backend.build_runtime_strict_profile_claim_implementation_surface(
                results
            )
        ),
        "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface": (
            runtime_backend.build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface(
                results
            )
        ),
        "runtime_claim_publication_dashboard_schema_surface": (
            runtime_backend.build_runtime_claim_publication_dashboard_schema_surface(
                results
            )
        ),
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
            runtime_backend.build_runtime_final_claim_publication_deprecated_path_shutdown_surface(
                results
            )
        ),
        "runtime_release_candidate_claim_abi_surface": (
            runtime_backend.build_runtime_release_candidate_claim_abi_surface(results)
        ),
        "runtime_final_release_evidence_descaffolding_implementation_surface": (
            runtime_backend.build_runtime_final_release_evidence_descaffolding_implementation_surface(
                results
            )
        ),
    }


__all__ = ["build_live_surfaces"]
