"""Security-hardening validation actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..composite_validation import run_composite_validation
from .release_governance_security_paths import (
    SECURITY_HARDENING_END_TO_END_PY,
    SECURITY_HARDENING_POSTURE_PY,
    SECURITY_HARDENING_PUBLICATION_PY,
    SECURITY_HARDENING_RESPONSE_DRILL_PY,
    SECURITY_HARDENING_RUNTIME_HARDENING_PY,
    SECURITY_HARDENING_SOURCE_SURFACE_PY,
)
from .schema_surfaces import SECURITY_HARDENING_SCHEMA_SURFACE_PY


def action_validate_security_hardening(_: list[str]) -> int:
    return run_composite_validation(
        "validate-security-hardening",
        [
            (
                "check-security-response-drill",
                [sys.executable, str(SECURITY_HARDENING_RESPONSE_DRILL_PY)],
            ),
            (
                "check-security-runtime-hardening",
                [sys.executable, str(SECURITY_HARDENING_RUNTIME_HARDENING_PY)],
            ),
            (
                "check-security-hardening-surface",
                [sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)],
            ),
            (
                "check-security-hardening-schema-surface",
                [sys.executable, str(SECURITY_HARDENING_SCHEMA_SURFACE_PY)],
            ),
            ("build-security-posture", [sys.executable, str(SECURITY_HARDENING_POSTURE_PY)]),
            (
                "publish-security-advisories",
                [sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)],
            ),
        ],
    )


def action_validate_security_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_END_TO_END_PY)])
