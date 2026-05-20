"""External validation publication action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

EXTERNAL_VALIDATION_PUBLICATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "publish-external-repro-corpus": ActionSpec(
        "publish-external-repro-corpus",
        "materialize the machine-owned external repro corpus publication artifact",
        "python:scripts/publish_objc3c_external_repro_corpus.py",
        validation_tier="repo",
        guarantee_owner=(
            "accepted and quarantined external evidence stays publishable as a "
            "machine-owned corpus summary rooted in the replay drill"
        ),
    ),
    "check-external-support-claim-gate": ActionSpec(
        "check-external-support-claim-gate",
        "gate support and adoption claims on accepted external replay evidence",
        "python:scripts/check_objc3c_external_support_claim_gate.py",
        validation_tier="repo",
        guarantee_owner=(
            "support and adoption claims fail closed when external validation "
            "evidence is missing stale non-reproducible or local-only"
        ),
    ),
}

__all__ = ["EXTERNAL_VALIDATION_PUBLICATION_ACTION_SPECS"]
