#!/usr/bin/env python3
"""Shared-harness-first readiness runner for M318-B003."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = [
    [sys.executable, str(ROOT / "scripts" / "check_m318_b003_maintainer_review_checklist_and_regression_handling_model_edge_case_and_compatibility_completion.py")],
    [sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m318_b003_maintainer_review_checklist_and_regression_handling_model_edge_case_and_compatibility_completion.py"), "-q"],
]


def main() -> int:
    for command in COMMANDS:
        result = subprocess.run(command, cwd=ROOT)
        if result.returncode != 0:
            return result.returncode
    print("M318-B003 lane-B readiness passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
