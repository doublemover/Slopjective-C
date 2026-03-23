#!/usr/bin/env python3
"""Lane-B readiness for M271-B002."""

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
    run([sys.executable, "scripts/run_m271_b001_lane_b_readiness.py"])
    run([sys.executable, "scripts/check_m271_b002_resource_move_and_use_after_move_semantics_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m271_b002_resource_move_and_use_after_move_semantics_core_feature_implementation.py", "-q"])
    print("[info] dependency continuity token: M271-B001 + M271-B002 (cleanup/resource locals now fail closed on non-resource move capture, use-after-move, and duplicate cleanup transfer while borrowed escape, retainable-family legality, lowering, and runtime remain later M271 work)")
    print("[ok] M271-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
