#!/usr/bin/env python3
"""Lane-B readiness for M272-B001."""

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
    run([sys.executable, "scripts/run_m272_a002_lane_a_readiness.py"])
    run([sys.executable, "scripts/check_m272_b001_dynamism_and_dispatch_control_semantic_model_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m272_b001_dynamism_and_dispatch_control_semantic_model_contract_and_architecture_freeze.py", "-q"])
    print("[info] dependency continuity token: M272-A002 + M272-B001 (Part 9 now preserves one truthful sema packet over dispatch-intent/defaulting counts plus override-accounting before later legality and lowering work)")
    print("[ok] M272-B001 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
