#!/usr/bin/env python3
"""Readiness runner for M265-B002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M265-B001 + M265-B002 (Part 3 optional bindings, refinement, and nil-coalescing now execute truthfully before later lowering/runtime expansion)")
    run(["python", "scripts/run_m265_b001_lane_b_readiness.py"])
    run(["python", "scripts/check_m265_b002_optional_flow_binding_and_refinement_semantics_core_feature_implementation.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m265_b002_optional_flow_binding_and_refinement_semantics_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M265-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
