"""Distribution credibility artifact workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_distribution_credibility_paths import (
    DISTRIBUTION_CREDIBILITY_DASHBOARD_PY,
    DISTRIBUTION_CREDIBILITY_PUBLICATION_PY,
    DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY,
)
from .release_governance_distribution_credibility_owner_contracts import (
    require_distribution_credibility_owner_contract,
)


def action_check_distribution_credibility_surface(_: list[str]) -> int:
    require_distribution_credibility_owner_contract("check-distribution-credibility-surface")
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY)])


def action_build_distribution_credibility_dashboard(_: list[str]) -> int:
    require_distribution_credibility_owner_contract("build-distribution-credibility-dashboard")
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_DASHBOARD_PY)])


def action_publish_distribution_credibility(_: list[str]) -> int:
    require_distribution_credibility_owner_contract("publish-distribution-credibility")
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_PUBLICATION_PY)])


__all__ = [
    "action_build_distribution_credibility_dashboard",
    "action_check_distribution_credibility_surface",
    "action_publish_distribution_credibility",
]
