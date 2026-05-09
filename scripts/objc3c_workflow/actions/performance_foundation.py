"""Benchmark foundation performance workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

PERFORMANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_performance_integration.py"


def action_validate_performance_foundation(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_INTEGRATION_PY)])


__all__ = [
    "PERFORMANCE_INTEGRATION_PY",
    "action_validate_performance_foundation",
]
