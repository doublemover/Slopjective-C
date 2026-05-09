"""Distribution credibility artifact workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_distribution_credibility_paths import (
    DISTRIBUTION_CREDIBILITY_DASHBOARD_PY,
    DISTRIBUTION_CREDIBILITY_PUBLICATION_PY,
    DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY,
)


def action_check_distribution_credibility_surface(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY)])


def action_build_distribution_credibility_dashboard(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_DASHBOARD_PY)])


def action_publish_distribution_credibility(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_PUBLICATION_PY)])


__all__ = [
    "action_build_distribution_credibility_dashboard",
    "action_check_distribution_credibility_surface",
    "action_publish_distribution_credibility",
]
