"""Performance governance source and schema policy workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_performance_governance_source_surface.py"
)
PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY = (
    ROOT / "scripts" / "check_performance_governance_schema_surface.py"
)


def action_check_performance_governance_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY)])


def action_check_performance_governance_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY)])


__all__ = [
    "PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY",
    "PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY",
    "action_check_performance_governance_schema_surface",
    "action_check_performance_governance_surface",
]
