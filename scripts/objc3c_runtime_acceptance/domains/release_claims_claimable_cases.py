"""Release-claims claimable feature source runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from ..case_result import CaseResult
from ..runtime_contract_release import RELEASE_CLAIMABLE_SURFACE_FIXTURE
from .release_claims_claimable_artifacts import compile_claimable_surface
from .release_claims_claimable_assertions import (
    EXPECTED_OPTIONAL_GAP_IDS,
    EXPECTED_TARGETED_PROFILES,
    expect_residual_non_claimable_gap_surfaces,
    expect_strict_profile_feature_claim_surfaces,
)
from .release_claims_owner_contracts import release_claims_case_summary


def check_claimable_surface_residual_non_claimable_gaps_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    artifacts = compile_claimable_surface(
        run_dir, "claimable-surface-residual-non-claimable-gaps-source-surface"
    )
    expect_residual_non_claimable_gap_surfaces(
        artifacts.report,
        artifacts.publication,
        artifacts.advanced_feature_gate,
        artifacts.release_candidate_matrix,
        artifacts.runtime_capability_report,
    )

    return CaseResult(
        case_id="claimable-surface-residual-non-claimable-gaps-source-surface",
        probe="compile-conformance-report-publication-runtime-capability-and-release-gate-sidecars",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=release_claims_case_summary(
            "claimable-surface-residual-non-claimable-gaps-source-surface",
            {
                "claimed_profiles": artifacts.runtime_capability_report.get(
                    "claimed_profile_ids"
                ),
                "selected_profile": artifacts.publication.get("selected_profile"),
                "unsupported_feature_claim_ids": artifacts.report.get(
                    "unsupported_feature_claim_ids"
                ),
                "optional_non_claimable_features": EXPECTED_OPTIONAL_GAP_IDS,
            },
        ),
    )


def check_strict_profile_feature_claim_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    artifacts = compile_claimable_surface(
        run_dir, "strict-profile-feature-claim-source-surface"
    )
    expect_strict_profile_feature_claim_surfaces(
        artifacts.publication,
        artifacts.advanced_feature_gate,
        artifacts.release_candidate_matrix,
        artifacts.feature_claim_truth_surface,
        artifacts.compatibility_semantics,
    )

    return CaseResult(
        case_id="strict-profile-feature-claim-source-surface",
        probe="compile-conformance-report-publication-and-release-gate-targeting-sidecars",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=release_claims_case_summary(
            "strict-profile-feature-claim-source-surface",
            {
                "supported_selection_surface_ids": artifacts.feature_claim_truth_surface.get(
                    "supported_selection_surface_ids"
                ),
                "unsupported_selection_surface_ids": artifacts.feature_claim_truth_surface.get(
                    "unsupported_selection_surface_ids"
                ),
                "targeted_profile_ids": EXPECTED_TARGETED_PROFILES,
                "rejection_model": artifacts.compatibility_semantics.get(
                    "rejection_model"
                ),
            },
        ),
    )


__all__ = [
    "check_claimable_surface_residual_non_claimable_gaps_source_surface_case",
    "check_strict_profile_feature_claim_source_surface_case",
]
