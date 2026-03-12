#!/usr/bin/env python3
"""Readiness runner for M264-C001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-B003 + M276-D001 + M264-C001 (truthful frontend packets must lower into one machine-readable conformance sidecar without overstating unsupported surfaces)")
    run(["python", "scripts/run_m264_b003_lane_b_readiness.py"])
    run(["python", "scripts/check_m264_c001_versioned_conformance_report_lowering_contract_and_architecture_freeze.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_c001_versioned_conformance_report_lowering_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M264-C001 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
