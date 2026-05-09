"""External validation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

EXTERNAL_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = {
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
    "test-external-validation-replay": ActionSpec(
        "test-external-validation-replay",
        "run accepted external-validation entries through the live replay proof scripts",
        "python:scripts/run_objc3c_external_validation_replay.py",
        validation_tier="repo",
        guarantee_owner=(
            "accepted external evidence keeps replaying through the live parser and "
            "execution proof surfaces"
        ),
    ),
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
