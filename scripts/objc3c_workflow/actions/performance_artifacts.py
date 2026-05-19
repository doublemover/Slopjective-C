"""Performance artifact and report assembly workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

PERFORMANCE_GOVERNANCE_DASHBOARD_PY = (
    ROOT / "scripts" / "build_objc3c_performance_dashboard.py"
)
PERFORMANCE_GOVERNANCE_REPORT_PY = (
    ROOT / "scripts" / "publish_objc3c_performance_report.py"
)


def action_build_performance_dashboard(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_DASHBOARD_PY)])


def action_publish_performance_report(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_REPORT_PY)])
