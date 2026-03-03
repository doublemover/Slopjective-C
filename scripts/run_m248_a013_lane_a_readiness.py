#!/usr/bin/env python3
"""Run M248-A013 lane-A readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py",
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
    print("[ok] M248-A013 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
