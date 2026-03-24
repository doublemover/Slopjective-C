#!/usr/bin/env python3
"""Lane-B readiness for M272-B003."""

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
    run([sys.executable, "scripts/run_m272_b002_lane_b_readiness.py"])
    run([sys.executable, "scripts/check_m272_b003_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m272_b003_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion.py", "-q"])
    print("[info] dependency continuity token: M272-B002 + M272-B003 (Part 9 now preserves one truthful compatibility packet over callable conflict diagnostics and unsupported function/protocol/category dispatch-intent topologies before later lowering work)")
    print("[ok] M272-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
