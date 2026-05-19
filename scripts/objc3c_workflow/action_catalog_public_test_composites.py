"""Composite public test entrypoint specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PUBLIC_TEST_COMPOSITE_ACTION_SPECS: dict[str, ActionSpec] = {
    "test-default": ActionSpec(
        "test-default",
        "default public test entrypoint",
        "runner-internal smoke validation",
    ),
    "test-smoke": ActionSpec(
        "test-smoke",
        "smoke public validation entrypoint",
        "runner-internal + behavior matrix + runtime acceptance + replay",
        validation_tier="smoke",
        guarantee_owner=(
            "behavior matrix, runtime acceptance, and canonical replay through "
            "the public workflow"
        ),
    ),
    "test-ci": ActionSpec(
        "test-ci",
        "CI-oriented public validation entrypoint",
        "runner-internal + task hygiene",
        validation_tier="ci",
        guarantee_owner=(
            "task hygiene, developer-tooling integration, bonus-experience validation, "
            "stdlib validation, performance governance reporting, runtime acceptance, "
            "canonical replay, and full execution smoke validation"
        ),
    ),
    "test-full": ActionSpec(
        "test-full",
        "full developer validation entrypoint with bounded smoke slice",
        "runner-internal + runtime acceptance + replay + smoke -Limit 24",
        validation_tier="full",
        guarantee_owner=(
            "runtime acceptance, canonical replay, and a deterministic 24-fixture "
            "smoke slice; use test-smoke or test-nightly for exhaustive smoke"
        ),
    ),
    "test-nightly": ActionSpec(
        "test-nightly",
        "exhaustive validation entrypoint",
        "runner-internal nightly child actions",
        validation_tier="nightly",
        guarantee_owner=(
            "full validation plus performance governance reporting, release-foundation "
            "publication, conformance corpus indexing, recovery, and broad corpus sweeps"
        ),
    ),
}
