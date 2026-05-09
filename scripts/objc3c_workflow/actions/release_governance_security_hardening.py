"""Security hardening workflow action surface."""

from __future__ import annotations

from ..action_spec import ActionHandler
from .release_governance_security_artifacts import (
    action_build_security_posture,
    action_check_security_hardening_surface,
    action_publish_security_advisories,
)
from .release_governance_security_targets import (
    BUILD_SECURITY_POSTURE,
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
    CHECK_SECURITY_HARDENING_SURFACE,
    PUBLISH_SECURITY_ADVISORIES,
    VALIDATE_SECURITY_HARDENING,
    VALIDATE_SECURITY_HARDENING_END_TO_END,
)
from .release_governance_security_validation import (
    action_validate_security_hardening,
    action_validate_security_hardening_end_to_end,
)
from .schema_surfaces import action_check_security_hardening_schema_surface

SECURITY_HARDENING_ACTION_HANDLERS: dict[str, ActionHandler] = {
    CHECK_SECURITY_HARDENING_SURFACE: action_check_security_hardening_surface,
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE: action_check_security_hardening_schema_surface,
    BUILD_SECURITY_POSTURE: action_build_security_posture,
    PUBLISH_SECURITY_ADVISORIES: action_publish_security_advisories,
    VALIDATE_SECURITY_HARDENING: action_validate_security_hardening,
    VALIDATE_SECURITY_HARDENING_END_TO_END: action_validate_security_hardening_end_to_end,
}

__all__ = [
    "SECURITY_HARDENING_ACTION_HANDLERS",
    "action_build_security_posture",
    "action_check_security_hardening_schema_surface",
    "action_check_security_hardening_surface",
    "action_publish_security_advisories",
    "action_validate_security_hardening",
    "action_validate_security_hardening_end_to_end",
]
