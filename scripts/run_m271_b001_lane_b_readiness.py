#!/usr/bin/env python3
"""Lane-B readiness for M271-B001."""

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
    run([sys.executable, "scripts/run_m271_a003_lane_a_readiness.py"])
    run([sys.executable, "scripts/check_m271_b001_system_extension_semantic_model_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m271_b001_system_extension_semantic_model_contract_and_architecture_freeze.py", "-q"])
    print("[info] dependency continuity token: M271-A003 + M271-B001 (cleanup/resource locals, borrowed pointers, capture lists, and retainable-family declarations now share one truthful Part 8 sema packet before later M271 legality and runtime work)")
    print("[ok] M271-B001 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
