#!/usr/bin/env python3
"""Readiness runner for M264-A002."""

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
    print("[info] dependency continuity token: M264-A001 + M263-E002 + M264-A002 (selection and claim truth surfaces must remain bounded by the live runnable frontend subset)")
    run(["python", "scripts/run_m264_a001_lane_a_readiness.py"])
    run(["python", "scripts/check_m264_a002_frontend_feature_claim_and_strictness_truthfulness_wiring_core_feature_implementation.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_a002_frontend_feature_claim_and_strictness_truthfulness_wiring_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M264-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
