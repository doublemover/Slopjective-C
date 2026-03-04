#!/usr/bin/env python3
"""Run M249-E019 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m249_e018_lane_e_readiness.py",
    ),
    (
        "npm.cmd",
        "run",
        "check:objc3c:m249-a007-lane-a-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m249_b009_lane_b_readiness.py",
    ),
    (
        "npm.cmd",
        "run",
        "check:objc3c:m249-c010-lane-c-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m249_d016_lane_d_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m249_e019_lane_e_release_gate_docs_and_runbooks_advanced_integration_workpack_shard1_contract.py",
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
    print("[ok] M249-E019 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
