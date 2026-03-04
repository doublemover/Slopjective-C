#!/usr/bin/env python3
"""Run M247-A005 lane-A readiness checks with deterministic dependency chaining."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m247_a004_lane_a_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m247_a005_frontend_profiling_and_hot_path_decomposition_edge_case_and_compatibility_completion_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m247_a005_frontend_profiling_and_hot_path_decomposition_edge_case_and_compatibility_completion_contract.py",
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
    print("[ok] M247-A005 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())


