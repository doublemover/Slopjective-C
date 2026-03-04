#!/usr/bin/env python3
"""Run M248-E030 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m248_e029_lane_e_readiness.py",
    ),
    (
        "npm.cmd",
        "run",
        "check:objc3c:m248-a011-lane-a-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m248_b014_lane_b_readiness.py",
    ),
    (
        sys.executable,
        "scripts/run_m248_c016_lane_c_readiness.py",
    ),
    (
        sys.executable,
        "scripts/run_m248_d021_lane_d_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m248_e030_ci_governance_gate_and_closeout_policy_advanced_conformance_workpack_shard3_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_e030_ci_governance_gate_and_closeout_policy_advanced_conformance_workpack_shard3_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M248-E030 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())


