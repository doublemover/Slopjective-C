#!/usr/bin/env python3
"""Readiness runner for M318-E002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    commands = [
        [sys.executable, "scripts/check_m318_e002_governance_hardening_closeout_matrix_cross_lane_integration_sync.py"],
        [sys.executable, "-m", "pytest", "tests/tooling/test_check_m318_e002_governance_hardening_closeout_matrix_cross_lane_integration_sync.py", "-q"],
    ]
    for command in commands:
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            return completed.returncode
    print("M318-E002 lane-E readiness passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
