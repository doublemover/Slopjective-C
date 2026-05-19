"""External validation replay action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

EXTERNAL_VALIDATION_REPLAY_ACTION_SPECS: dict[str, ActionSpec] = {
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
}

__all__ = ["EXTERNAL_VALIDATION_REPLAY_ACTION_SPECS"]
