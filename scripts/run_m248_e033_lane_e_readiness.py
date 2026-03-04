#!/usr/bin/env python3
"""Run M248-E033 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        "npm.cmd",
        "run",
        "check:objc3c:m248-e032-lane-e-readiness",
    ),
    (
        "npm.cmd",
        "run",
        "check:objc3c:m248-a012-lane-a-readiness",
    ),
    (
        "npm.cmd",
        "run",
        "check:objc3c:m248-b015-lane-b-readiness",
    ),
    (
        "npm.cmd",
        "run",
        "check:objc3c:m248-c018-lane-c-readiness",
    ),
    (
        "npm.cmd",
        "run",
        "check:objc3c:m248-d024-lane-d-readiness",
    ),
    (
        sys.executable,
        "scripts/check_m248_e033_ci_governance_gate_and_closeout_policy_advanced_core_workpack_shard4_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_e033_ci_governance_gate_and_closeout_policy_advanced_core_workpack_shard4_contract.py",
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
    print("[ok] M248-E033 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
