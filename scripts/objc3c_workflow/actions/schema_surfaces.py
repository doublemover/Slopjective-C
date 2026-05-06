"""Schema-surface workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_public_conformance_schema_surface.py"
)
RELEASE_FOUNDATION_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_release_foundation_schema_surface.py"
)
PACKAGING_CHANNELS_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_packaging_channels_schema_surface.py"
)
RELEASE_OPERATIONS_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_release_operations_schema_surface.py"
)
DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_distribution_credibility_schema_surface.py"
)
SECURITY_HARDENING_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_security_hardening_schema_surface.py"
)


def action_check_public_conformance_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY)])


def action_check_release_foundation_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_FOUNDATION_SCHEMA_SURFACE_PY)])


def action_check_packaging_channels_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_SCHEMA_SURFACE_PY)])


def action_check_release_operations_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_SCHEMA_SURFACE_PY)])


def action_check_distribution_credibility_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY)])


def action_check_security_hardening_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_SCHEMA_SURFACE_PY)])
