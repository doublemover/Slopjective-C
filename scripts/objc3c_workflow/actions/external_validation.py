"""External validation workflow actions."""

from __future__ import annotations

from .external_validation_execution import (
    run_external_validation_target,
    run_validate_external_validation,
    run_validate_external_validation_integration,
)
from .external_validation_targets import (
    EXTERNAL_VALIDATION_INTEGRATION_PY,
    EXTERNAL_VALIDATION_PUBLICATION_PY,
    EXTERNAL_VALIDATION_REPLAY_PY,
    EXTERNAL_VALIDATION_SURFACE_PY,
    EXTERNAL_VALIDATION_TARGETS,
    ExternalValidationTarget,
)


def action_check_external_validation_surface(_: list[str]) -> int:
    return run_external_validation_target("check-external-validation-surface")


def action_test_external_validation_replay(_: list[str]) -> int:
    return run_external_validation_target("test-external-validation-replay")


def action_publish_external_repro_corpus(_: list[str]) -> int:
    return run_external_validation_target("publish-external-repro-corpus")


def action_validate_external_validation(_: list[str]) -> int:
    return run_validate_external_validation()


def action_validate_external_validation_integration(_: list[str]) -> int:
    return run_validate_external_validation_integration()


__all__ = [
    "EXTERNAL_VALIDATION_INTEGRATION_PY",
    "EXTERNAL_VALIDATION_PUBLICATION_PY",
    "EXTERNAL_VALIDATION_REPLAY_PY",
    "EXTERNAL_VALIDATION_SURFACE_PY",
    "EXTERNAL_VALIDATION_TARGETS",
    "ExternalValidationTarget",
    "action_check_external_validation_surface",
    "action_publish_external_repro_corpus",
    "action_test_external_validation_replay",
    "action_validate_external_validation",
    "action_validate_external_validation_integration",
]
