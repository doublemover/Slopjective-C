#!/usr/bin/env python3
"""Readiness runner for M264-D001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-C002 + M264-D001 (driver publication stays a truthful projection of the lowered conformance/runtime capability sidecar)")
    run(["python", "scripts/run_m264_c002_lane_c_readiness.py"])
    run(["python", "scripts/check_m264_d001_driver_and_profile_selection_report_publication_contract_and_architecture_freeze.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_d001_driver_and_profile_selection_report_publication_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M264-D001 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
