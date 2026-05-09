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
}

__all__ = ["EXTERNAL_VALIDATION_PUBLICATION_ACTION_SPECS"]
