#!/usr/bin/env python3
"""Lane-B readiness for M272-B002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    run([sys.executable, "scripts/run_m272_b001_lane_b_readiness.py"])
    run([sys.executable, "scripts/check_m272_b002_override_finality_and_sealing_legality_enforcement_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m272_b002_override_finality_and_sealing_legality_enforcement_core_feature_implementation.py", "-q"])
    print("[info] dependency continuity token: M272-B001 + M272-B002 (Part 9 now preserves one truthful legality packet over superclass finality/sealing and direct/final override restrictions before later lowering work)")
    print("[ok] M272-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
