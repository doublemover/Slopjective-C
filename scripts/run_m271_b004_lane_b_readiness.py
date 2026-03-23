#!/usr/bin/env python3
"""Lane-B readiness for M271-B004."""

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
    run([sys.executable, "scripts/run_m271_b003_lane_b_readiness.py"])
    run([sys.executable, "scripts/check_m271_b004_capture_list_and_retainable_family_legality_completion_edge_case_and_compatibility_completion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m271_b004_capture_list_and_retainable_family_legality_completion_edge_case_and_compatibility_completion.py", "-q"])
    print("[info] dependency continuity token: M271-B003 + M271-B004 (capture-list edge cases and retainable-family compatibility aliases now share one truthful Part 8 sema packet before later lowering/runtime work)")
    print("[ok] M271-B004 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

