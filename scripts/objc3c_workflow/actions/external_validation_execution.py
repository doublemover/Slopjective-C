"""External validation command execution and action glue."""

from __future__ import annotations

import sys

from ..commands import run
from ..composite_validation import run_composite_validation
from .external_validation_targets import (
    EXTERNAL_VALIDATION_INTEGRATION_PY,
    VALIDATE_EXTERNAL_VALIDATION_CHILD_ACTIONS,
    external_validation_target,
)


def external_validation_command(action_name: str) -> list[str]:
    target = external_validation_target(action_name)
    return [sys.executable, str(target.script)]


def run_external_validation_target(action_name: str) -> int:
    return run(external_validation_command(action_name))


def run_validate_external_validation() -> int:
    return run_composite_validation(
        "validate-external-validation",
        [
            (action_name, external_validation_command(action_name))
            for action_name in VALIDATE_EXTERNAL_VALIDATION_CHILD_ACTIONS
        ],
    )


def run_validate_external_validation_integration() -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_INTEGRATION_PY)])
