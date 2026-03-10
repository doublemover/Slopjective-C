#!/usr/bin/env python3
"""Readiness runner for M264-B002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-B001 + M264-B002 (truthful semantic claim packets must fail closed for accepted but non-runnable source surfaces before lowering/runtime handoff)")
    run(["python", "scripts/run_m264_b001_lane_b_readiness.py"])
    run(["python", "scripts/check_m264_b002_fail_closed_unsupported_feature_claim_enforcement_core_feature_implementation.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_b002_fail_closed_unsupported_feature_claim_enforcement_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M264-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
