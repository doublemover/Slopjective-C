#!/usr/bin/env python3
"""Lane-E readiness runner for M264-E001."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

ROOT = None


def run(command: Sequence[str]) -> None:
    completed = subprocess.run(list(command), check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-A002 + M264-B003 + M264-C002 + M264-D002 (lane-E freezes one integrated core/json-only conformance boundary)", flush=True)
    run(["python", "scripts/run_m264_a002_lane_a_readiness.py"])
    run(["python", "scripts/run_m264_b003_lane_b_readiness.py"])
    run(["python", "scripts/run_m264_c002_lane_c_readiness.py"])
    run(["python", "scripts/run_m264_d002_lane_d_readiness.py"])
    run(["python", "scripts/check_m264_e001_versioning_and_conformance_truth_gate_contract_and_architecture_freeze.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_e001_versioning_and_conformance_truth_gate_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M264-E001 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
