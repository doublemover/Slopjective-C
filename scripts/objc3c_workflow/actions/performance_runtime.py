"""Runtime-performance workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

RUNTIME_PERFORMANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_runtime_performance_integration.py"
)


def action_validate_runtime_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_PERFORMANCE_INTEGRATION_PY)])


__all__ = [
    "RUNTIME_PERFORMANCE_INTEGRATION_PY",
    "action_validate_runtime_performance",
]
