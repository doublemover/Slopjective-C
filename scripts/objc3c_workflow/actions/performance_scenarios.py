"""Performance scenario-selection workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

COMPARATIVE_BASELINES_PY = ROOT / "scripts" / "run_objc3c_comparative_baselines.py"


def action_benchmark_comparative_baselines(rest: list[str]) -> int:
    return run([sys.executable, str(COMPARATIVE_BASELINES_PY), *rest])
