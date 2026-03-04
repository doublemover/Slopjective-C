#!/usr/bin/env python3
"""Run M246-A006 lane-A readiness checks with deterministic dependency chaining."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m246_a005_lane_a_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py",
        "--emit-json",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py",
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
    print("[ok] M246-A006 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
