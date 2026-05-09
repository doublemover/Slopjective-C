"""Security-hardening artifact actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_security_paths import (
    SECURITY_HARDENING_POSTURE_PY,
    SECURITY_HARDENING_PUBLICATION_PY,
    SECURITY_HARDENING_SOURCE_SURFACE_PY,
)


def action_check_security_hardening_surface(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)])


def action_build_security_posture(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_POSTURE_PY)])


def action_publish_security_advisories(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)])
