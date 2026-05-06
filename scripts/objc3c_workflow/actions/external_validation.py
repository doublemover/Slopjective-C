"""External validation workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

EXTERNAL_VALIDATION_SURFACE_PY = ROOT / "scripts" / "check_external_validation_source_surface.py"
EXTERNAL_VALIDATION_REPLAY_PY = ROOT / "scripts" / "run_objc3c_external_validation_replay.py"
EXTERNAL_VALIDATION_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_external_repro_corpus.py"
EXTERNAL_VALIDATION_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_external_validation_integration.py"
)


def action_check_external_validation_surface(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_SURFACE_PY)])


def action_test_external_validation_replay(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_REPLAY_PY)])


def action_publish_external_repro_corpus(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_PUBLICATION_PY)])


def action_validate_external_validation(_: list[str]) -> int:
    from scripts.objc3c_workflow.runner import run_composite_validation

    return run_composite_validation(
        "validate-external-validation",
        [
            (
                "check-external-validation-surface",
                [sys.executable, str(EXTERNAL_VALIDATION_SURFACE_PY)],
            ),
            (
                "test-external-validation-replay",
                [sys.executable, str(EXTERNAL_VALIDATION_REPLAY_PY)],
            ),
            (
                "publish-external-repro-corpus",
                [sys.executable, str(EXTERNAL_VALIDATION_PUBLICATION_PY)],
            ),
        ],
    )


def action_validate_external_validation_integration(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_INTEGRATION_PY)])
