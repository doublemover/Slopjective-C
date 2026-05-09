"""Public-conformance reporting artifact actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_public_conformance_paths import (
    PUBLIC_CONFORMANCE_REPORT_PY,
    PUBLIC_CONFORMANCE_SCORECARD_PY,
    PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY,
)


def action_check_public_conformance_reporting_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY)])


def action_build_public_conformance_scorecard(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SCORECARD_PY)])


def action_publish_public_conformance_report(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_REPORT_PY)])
