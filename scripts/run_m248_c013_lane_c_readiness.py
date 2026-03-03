#!/usr/bin/env python3
"""Run M248-C013 lane-C readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py",
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
    print("[ok] M248-C013 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
