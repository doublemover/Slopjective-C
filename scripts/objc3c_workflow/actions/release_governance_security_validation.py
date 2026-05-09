"""Security-hardening validation actions."""

from __future__ import annotations

from ..commands import run
from ..composite_validation import run_composite_validation
from .release_governance_security_targets import (
    SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS,
    VALIDATE_SECURITY_HARDENING_END_TO_END,
    security_hardening_command,
)


def action_validate_security_hardening(_: list[str]) -> int:
    return run_composite_validation(
        "validate-security-hardening",
        [
            (action_name, security_hardening_command(action_name))
            for action_name in SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS
        ],
    )


def action_validate_security_hardening_end_to_end(_: list[str]) -> int:
    return run(security_hardening_command(VALIDATE_SECURITY_HARDENING_END_TO_END))
