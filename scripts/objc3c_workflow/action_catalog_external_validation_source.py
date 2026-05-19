"""External validation source-surface action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

EXTERNAL_VALIDATION_SOURCE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-external-validation-surface": ActionSpec(
        "check-external-validation-surface",
        "validate the checked-in external-validation source, trust, intake, quarantine, and artifact contracts",
        "python:scripts/check_external_validation_source_surface.py",
        validation_tier="repo",
        guarantee_owner=(
            "external validation stays rooted in checked-in trust, intake, quarantine, "
            "and artifact contracts"
        ),
    ),
}

__all__ = ["EXTERNAL_VALIDATION_SOURCE_ACTION_SPECS"]
