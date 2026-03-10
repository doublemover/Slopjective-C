#!/usr/bin/env python3
"""Readiness runner for M264-B001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-A002 + M259-E002 + M263-E002 + M264-B001 (truthful frontend claim surfaces must flow into fail-closed sema legality semantics)")
    run(["python", "scripts/run_m264_a002_lane_a_readiness.py"])
    run(["python", "scripts/check_m264_b001_compatibility_strictness_and_claim_semantics_contract_and_architecture_freeze.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_b001_compatibility_strictness_and_claim_semantics_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M264-B001 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
