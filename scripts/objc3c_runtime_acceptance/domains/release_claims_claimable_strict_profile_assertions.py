"""Assertions for release-claims strict-profile feature claim surfaces."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect
from .release_claims_claimable_residual_assertions import EXPECTED_CLAIMED_PROFILES
from .release_claims_claimable_residual_assertions import EXPECTED_REJECTED_PROFILES
from .release_claims_claimable_residual_assertions import EXPECTED_TARGETED_PROFILES


def expect_strict_profile_feature_claim_surfaces(
    publication: dict[str, Any],
    advanced_feature_gate: dict[str, Any],
    release_candidate_matrix: dict[str, Any],
    feature_claim_truth_surface: dict[str, Any],
    canonical_selection_claim_semantics: dict[str, Any],
) -> None:
    expect(
        feature_claim_truth_surface.get("contract_id")
        == "objc3c.feature.claim.strictness.truth.surface.v1",
        "expected conformance report to embed the strictness and feature-claim truth surface",
    )
    expect(
        feature_claim_truth_surface.get("supported_selection_surface_ids")
        == [
            "selection:language-version",
            "selection:language-profile",
        ],
        "expected feature-claim truth surface to preserve the live supported selection set",
    )
    expect(
        feature_claim_truth_surface.get("unsupported_selection_surface_ids")
        == [
            "selection:strictness",
            "selection:strict-concurrency",
            "selection:canonical-rejection-diagnostics",
        ],
        "expected feature-claim truth surface to preserve the fail-closed selection set",
    )
    expect(
        feature_claim_truth_surface.get("strictness_selection_supported") is False
        and feature_claim_truth_surface.get("strict_concurrency_selection_supported")
        is False
        and feature_claim_truth_surface.get("feature_macro_surface_supported") is False
        and feature_claim_truth_surface.get("claim_truth_fail_closed") is True,
        "expected feature-claim truth surface to publish fail-closed strictness, strict-concurrency, and macro behavior",
    )
    expect(
        canonical_selection_claim_semantics.get("contract_id")
        == "objc3c.canonical.selection.claim.semantics.v1",
        "expected conformance report to embed the canonical selection claim semantics surface",
    )
    expect(
        canonical_selection_claim_semantics.get("rejection_model")
        == "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed",
        "expected canonical selection claim semantics to preserve the strict-profile rejection model",
    )
    expect(
        canonical_selection_claim_semantics.get("fail_closed") is True
        and canonical_selection_claim_semantics.get(
            "strictness_selection_rejection_semantics_landed"
        )
        is True
        and canonical_selection_claim_semantics.get(
            "feature_macro_claim_suppression_semantics_landed"
        )
        is True
        and canonical_selection_claim_semantics.get("ready_for_lowering_and_runtime")
        is True,
        "expected canonical selection claim semantics to preserve a ready fail-closed strict-profile boundary",
    )
    expect(
        publication.get("supported_profile_ids") == EXPECTED_CLAIMED_PROFILES
        and publication.get("rejected_profile_ids") == EXPECTED_REJECTED_PROFILES,
        "expected publication to preserve the core-only claim set and fail-closed strict profile inventory",
    )
    expect(
        publication.get("advanced_feature_targeted_profile_ids")
        == EXPECTED_TARGETED_PROFILES
        and advanced_feature_gate.get("targeted_profile_ids")
        == EXPECTED_TARGETED_PROFILES
        and release_candidate_matrix.get("targeted_profile_ids")
        == EXPECTED_TARGETED_PROFILES,
        "expected publication, advanced feature gate, and release-candidate matrix to preserve one strict-profile targeting set",
    )


__all__ = [
    "expect_strict_profile_feature_claim_surfaces",
]
