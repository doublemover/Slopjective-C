"""Release-claim case factories."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories


def build_release_claim_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "claimable-surface-residual-non-claimable-gaps-source-surface",
            lambda: domains.release_claims.check_claimable_surface_residual_non_claimable_gaps_source_surface_case(
                run_dir
            ),
        ),
        (
            "strict-profile-feature-claim-source-surface",
            lambda: domains.release_claims.check_strict_profile_feature_claim_source_surface_case(
                run_dir
            ),
        ),
        (
            "claimability-semantics-release-policy",
            lambda: domains.release_claims.check_claimability_semantics_release_policy_case(
                run_dir
            ),
        ),
        (
            "strict-profile-claim-implementation",
            lambda: domains.release_claims.check_strict_profile_claim_implementation_case(
                run_dir
            ),
        ),
        (
            "retired-artifact-rejection-contracts",
            lambda: domains.release_claims.check_retired_artifact_rejection_contracts_case(
                run_dir
            ),
        ),
        (
            "claim-publication-dashboard-schema-surface",
            lambda: domains.release_claims.check_claim_publication_dashboard_schema_surface_case(
                run_dir
            ),
        ),
        (
            "final-claim-publication-deprecated-path-shutdown",
            lambda: domains.release_claims.check_final_claim_publication_deprecated_path_shutdown_case(
                run_dir
            ),
        ),
        (
            "release-candidate-runtime-claim-abi",
            lambda: domains.release_claims.check_release_candidate_runtime_claim_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "current-release-evidence-owner-payload",
            lambda: domains.release_claims.check_current_release_evidence_owner_payload_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


__all__ = ["build_release_claim_case_factories"]
