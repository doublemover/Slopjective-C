#!/usr/bin/env python3
"""Run M247-A001 lane-A readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py",
        "--emit-json",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py",
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
    print("[ok] M247-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
