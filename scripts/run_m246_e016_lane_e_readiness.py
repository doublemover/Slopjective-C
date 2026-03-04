#!/usr/bin/env python3
"""Run M246-E016 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"

BASELINE_DEPENDENCIES = ("M246-E015", "M246-A012", "M246-B017", "M246-D012")
PENDING_SEEDED_DEPENDENCY_TOKENS = ("M246-C029",)
D011_REAL_READINESS_SCRIPT = "scripts/run_m246_d012_lane_d_readiness.py"

INITIAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m246_e015_lane_e_readiness.py",
    ),
    (
        sys.executable,
        "scripts/run_m246_a012_lane_a_readiness.py",
    ),
    (
        sys.executable,
        "scripts/run_m246_b017_lane_b_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m246-c029-lane-c-readiness",
    ),
    (
        sys.executable,
        D011_REAL_READINESS_SCRIPT,
    ),
)

FINAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m246_e016_optimization_gate_and_perf_evidence_integration_closeout_and_gate_sign_off_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_e016_optimization_gate_and_perf_evidence_integration_closeout_and_gate_sign_off_contract.py",
        "-q",
    ),
)


def run_command(command: Sequence[str]) -> int:
    command_text = " ".join(command)
    print(f"[run] {command_text}")
    completed = subprocess.run(command, check=False)
    if completed.returncode != 0:
        print(
            f"[error] command failed with exit code {completed.returncode}: {command_text}",
            file=sys.stderr,
        )
    return completed.returncode


def run_chain() -> int:
    print(f"[info] baseline dependencies: {', '.join(BASELINE_DEPENDENCIES)}")
    print(
        "[info] pending seeded dependency tokens: "
        f"{', '.join(PENDING_SEEDED_DEPENDENCY_TOKENS)}"
    )
    for command in INITIAL_COMMAND_CHAIN:
        exit_code = run_command(command)
        if exit_code != 0:
            return exit_code

    for command in FINAL_COMMAND_CHAIN:
        exit_code = run_command(command)
        if exit_code != 0:
            return exit_code

    print("[ok] M246-E016 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())

