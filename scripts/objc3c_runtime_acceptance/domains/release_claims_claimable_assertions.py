"""Release-claims claimable feature assertion exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.release_claims_claimable_residual_assertions import (
    EXPECTED_CLAIMED_PROFILES,
    EXPECTED_OPTIONAL_GAP_IDS,
    expect_residual_non_claimable_gap_surfaces,
)
from objc3c_runtime_acceptance.domains.release_claims_claimable_strict_profile_assertions import (
    EXPECTED_TARGETED_PROFILES,
    expect_strict_profile_feature_claim_surfaces,
)


__all__ = [
    "EXPECTED_CLAIMED_PROFILES",
    "EXPECTED_OPTIONAL_GAP_IDS",
    "EXPECTED_TARGETED_PROFILES",
    "expect_residual_non_claimable_gap_surfaces",
    "expect_strict_profile_feature_claim_surfaces",
]
