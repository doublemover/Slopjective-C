"""Security-hardening artifact actions."""

from __future__ import annotations

from ..commands import run
from .release_governance_security_targets import (
    BUILD_SECURITY_POSTURE,
    CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL,
    CHECK_SECURITY_HARDENING_SURFACE,
    CHECK_SECURITY_SANITIZER_VALIDATION,
    PUBLISH_SECURITY_ADVISORIES,
    security_hardening_command,
)


def action_check_security_hardening_surface(_: list[str]) -> int:
    return run(security_hardening_command(CHECK_SECURITY_HARDENING_SURFACE))


def action_check_security_sanitizer_validation(_: list[str]) -> int:
    return run(security_hardening_command(CHECK_SECURITY_SANITIZER_VALIDATION))


def action_check_security_language_runtime_threat_model(_: list[str]) -> int:
    return run(security_hardening_command(CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL))


def action_build_security_posture(_: list[str]) -> int:
    return run(security_hardening_command(BUILD_SECURITY_POSTURE))


def action_publish_security_advisories(_: list[str]) -> int:
    return run(security_hardening_command(PUBLISH_SECURITY_ADVISORIES))
