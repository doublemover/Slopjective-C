#!/usr/bin/env python3
"""Run M248-E014 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import shutil
import subprocess
import sys
from typing import Sequence


NPM = shutil.which("npm") or "npm"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m248_e013_lane_e_readiness.py",
    ),
    (
        NPM,
        "run",
        "check:objc3c:m248-a005-lane-a-readiness",
    ),
    (
        NPM,
        "run",
        "check:objc3c:m248-b006-lane-b-readiness",
    ),
    (
        NPM,
        "run",
        "check:objc3c:m248-c008-lane-c-readiness",
    ),
    (
        NPM,
        "run",
        "check:objc3c:m248-d010-lane-d-readiness",
    ),
    (
        sys.executable,
        "scripts/check_m248_e014_ci_governance_gate_and_closeout_policy_release_candidate_and_replay_dry_run_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_e014_ci_governance_gate_and_closeout_policy_release_candidate_and_replay_dry_run_contract.py",
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
    print("[ok] M248-E014 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
