"""Integrated external validation workflow action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

EXTERNAL_VALIDATION_WORKFLOW_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-external-validation": ActionSpec(
        "validate-external-validation",
        "run the integrated external-validation source, replay, and publication drill",
        "runner-internal external-validation child actions",
        validation_tier="repo",
        guarantee_owner=(
            "external evidence intake, replay, and publication stay executable on "
            "the live workflow"
        ),
    ),
    "validate-external-validation-integration": ActionSpec(
        "validate-external-validation-integration",
        "validate the integrated external-validation report and child artifact set",
        "python:scripts/check_objc3c_external_validation_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "integrated external-validation reports stay coherent across source-surface, "
            "replay, and publication outputs"
        ),
    ),
}

__all__ = ["EXTERNAL_VALIDATION_WORKFLOW_ACTION_SPECS"]
