#!/usr/bin/env python3
"""Run M247-C001 lane-C readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    print("[info] dependency continuity token: none (M247-C001 contract-freeze baseline)")
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
    print("[ok] M247-C001 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())

