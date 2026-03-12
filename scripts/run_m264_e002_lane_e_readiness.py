#!/usr/bin/env python3
"""Lane-E readiness runner for M264-E002."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


def run(command: Sequence[str]) -> None:
    completed = subprocess.run(list(command), check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print(
        "[info] dependency continuity token: "
        "M264-A002 + M264-B003 + M264-C002 + M264-D002 + M264-E001 "
        "(lane-E closeout publishes one release/runtime claim matrix without widening the runnable core/json-only surface)",
        flush=True,
    )
    run(["python", "scripts/run_m264_e001_lane_e_readiness.py"])
    run(["python", "scripts/check_m264_e002_release_and_runtime_claim_matrix_cross_lane_integration_sync.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_e002_release_and_runtime_claim_matrix_cross_lane_integration_sync.py",
        "-q",
    ])
    print("[ok] M264-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
