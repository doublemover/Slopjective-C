"""Public-conformance stability policy contract."""

from __future__ import annotations

from .release_governance_public_conformance_ids import (
    PUBLIC_CONFORMANCE_STABILITY_POLICY_CONTRACT_ID,
)
from .release_governance_public_conformance_models import (
    PublicConformanceScoreBand,
    PublicConformanceStabilityPolicy,
)


PUBLIC_CONFORMANCE_STABILITY_POLICY = PublicConformanceStabilityPolicy(
    contract_id=PUBLIC_CONFORMANCE_STABILITY_POLICY_CONTRACT_ID,
    allowed_public_statuses=("pass", "caution", "blocked"),
    allowed_badges=("claim-ready", "provisional", "blocked"),
    score_bands=(
        PublicConformanceScoreBand("claim-ready", 95, 100, "claim-ready", "pass"),
        PublicConformanceScoreBand("provisional", 70, 94, "provisional", "caution"),
        PublicConformanceScoreBand("blocked", 0, 69, "blocked", "blocked"),
    ),
    fail_closed_conditions=(
        "corpus integration summary must report PASS",
        "external validation integration summary must report PASS",
        "checked-in dashboard and release-evidence schema anchors must exist",
        "public reporting cannot promote quarantined or blocked evidence into claim-ready status",
    ),
    capability_truth_rules=(
        "claim-ready means every upstream evidence owner reported PASS and no hard blocks remain",
        "provisional means publishable with explicit deductions and caution status",
        "blocked means the public artifact must report blocked and cannot imply conformance readiness",
    ),
)


__all__ = ("PUBLIC_CONFORMANCE_STABILITY_POLICY",)
