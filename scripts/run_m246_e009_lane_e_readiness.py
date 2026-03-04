#!/usr/bin/env python3
"""Run M246-E009 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"

BASELINE_DEPENDENCIES = ("M246-E008", "M246-A007", "M246-D007")
PENDING_SEEDED_DEPENDENCY_TOKENS = ("M246-B010", "M246-C016")
D007_REAL_READINESS_SCRIPT = "scripts/run_m246_d007_lane_d_readiness.py"

INITIAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m246_e008_lane_e_readiness.py",
    ),
    (
        sys.executable,
        "scripts/run_m246_a007_lane_a_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m246-b010-lane-b-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m246-c016-lane-c-readiness",
    ),
    (
        sys.executable,
        D007_REAL_READINESS_SCRIPT,
    ),
)

FINAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m246_e009_optimization_gate_and_perf_evidence_conformance_matrix_implementation_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_e009_optimization_gate_and_perf_evidence_conformance_matrix_implementation_contract.py",
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

    print("[ok] M246-E009 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
