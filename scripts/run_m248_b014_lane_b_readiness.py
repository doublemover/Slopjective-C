#!/usr/bin/env python3
"""Run M248-B014 lane-B readiness checks without deep npm nesting."""

from __future__ import annotations

import shutil
import subprocess
import sys
from typing import Sequence


POWERSHELL = shutil.which("powershell") or "powershell"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m248_b013_lane_b_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py",
        "-q",
    ),
    (
        POWERSHELL,
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        "scripts/run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1",
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
    print("[ok] M248-B014 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
