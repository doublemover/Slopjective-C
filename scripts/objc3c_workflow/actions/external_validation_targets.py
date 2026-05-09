"""External validation target definitions."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from ..environment import ROOT

EXTERNAL_VALIDATION_SURFACE_PY = (
    ROOT / "scripts" / "check_external_validation_source_surface.py"
)
EXTERNAL_VALIDATION_REPLAY_PY = (
    ROOT / "scripts" / "run_objc3c_external_validation_replay.py"
)
EXTERNAL_VALIDATION_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_external_repro_corpus.py"
)
EXTERNAL_VALIDATION_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_external_validation_integration.py"
)


@dataclass(frozen=True)
class ExternalValidationTarget:
    action_name: str
    script: Path


EXTERNAL_VALIDATION_TARGETS: dict[str, ExternalValidationTarget] = {
    "check-external-validation-surface": ExternalValidationTarget(
        "check-external-validation-surface",
        EXTERNAL_VALIDATION_SURFACE_PY,
    ),
    "test-external-validation-replay": ExternalValidationTarget(
        "test-external-validation-replay",
        EXTERNAL_VALIDATION_REPLAY_PY,
    ),
    "publish-external-repro-corpus": ExternalValidationTarget(
        "publish-external-repro-corpus",
        EXTERNAL_VALIDATION_PUBLICATION_PY,
    ),
}

VALIDATE_EXTERNAL_VALIDATION_CHILD_ACTIONS = (
    "check-external-validation-surface",
    "test-external-validation-replay",
    "publish-external-repro-corpus",
)
