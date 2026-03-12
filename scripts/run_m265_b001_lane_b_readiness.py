#!/usr/bin/env python3
"""Readiness runner for M265-B001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M265-A002 + M259-E002 + M263-E002 + M265-B001 (Part 3 source closures now flow into one fail-closed semantic model before lowering)")
    run(["python", "scripts/run_m265_a002_lane_a_readiness.py"])
    run(["python", "scripts/check_m265_b001_optional_generic_erasure_and_key_path_semantic_model_contract_and_architecture_freeze.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m265_b001_optional_generic_erasure_and_key_path_semantic_model_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M265-B001 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
