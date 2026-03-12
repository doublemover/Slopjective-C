#!/usr/bin/env python3
"""Lane-B readiness runner for M266-B002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = [
    [sys.executable, "scripts/check_m266_b002_guard_refinement_and_match_exhaustiveness_semantics_core_feature_implementation.py"],
    [sys.executable, "-m", "pytest", "tests/tooling/test_check_m266_b002_guard_refinement_and_match_exhaustiveness_semantics_core_feature_implementation.py", "-q"],
]


def main() -> int:
    for command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            return completed.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
