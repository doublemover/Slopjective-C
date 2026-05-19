"""Release-claim summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.cases import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.summary_owner_contracts import (
    build_domain_summary_owner_payload,
)


def build_release_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_release_summary_owner": build_domain_summary_owner_payload(
            owner_module="summary_release_sections",
            domain="release-claims",
        ),
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
            domains.release_claims.build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface(
                results
            )
        ),
        "runtime_strict_profile_feature_claim_source_surface": (
            domains.release_claims.build_runtime_strict_profile_feature_claim_source_surface(
                results
            )
        ),
        "runtime_claimability_semantics_release_policy_surface": (
            domains.release_claims.build_runtime_claimability_semantics_release_policy_surface(
                results
            )
        ),
        "runtime_strict_profile_claim_implementation_surface": (
            domains.release_claims.build_runtime_strict_profile_claim_implementation_surface(
                results
            )
        ),
        "runtime_retired_artifact_rejection_contracts_surface": (
            domains.release_claims.build_runtime_retired_artifact_rejection_contracts_surface(
                results
            )
        ),
        "runtime_claim_publication_dashboard_schema_surface": (
            domains.release_claims.build_runtime_claim_publication_dashboard_schema_surface(
                results
            )
        ),
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
            domains.release_claims.build_runtime_final_claim_publication_deprecated_path_shutdown_surface(
                results
            )
        ),
        "runtime_release_candidate_claim_abi_surface": (
            domains.release_claims.build_runtime_release_candidate_claim_abi_surface(
                results
            )
        ),
        "runtime_current_release_evidence_owner_payload_surface": (
            domains.release_claims.build_runtime_current_release_evidence_owner_payload_surface(
                results
            )
        ),
    }


__all__ = ["build_release_summary_sections"]
