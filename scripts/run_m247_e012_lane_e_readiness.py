#!/usr/bin/env python3
"""Run M247-E012 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"

BASELINE_DEPENDENCIES = ("M247-E011",)
PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A012", "M247-B014", "M247-C013", "M247-D010")

INITIAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m247_e011_lane_e_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-a012-lane-a-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-b014-lane-b-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-c013-lane-c-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-d010-lane-d-readiness",
    ),
)

FINAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m247_e012_performance_slo_gate_and_reporting_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m247_e012_performance_slo_gate_and_reporting_cross_lane_integration_sync_contract.py",
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

    print("[ok] M247-E012 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
