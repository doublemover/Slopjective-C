#!/usr/bin/env python3
"""Run M246-E013 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"

BASELINE_DEPENDENCIES = ("M246-E012", "M246-A010", "M246-D010")
PENDING_SEEDED_DEPENDENCY_TOKENS = ("M246-B014", "M246-C024")
D010_REAL_READINESS_SCRIPT = "scripts/run_m246_d010_lane_d_readiness.py"

INITIAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m246_e012_lane_e_readiness.py",
    ),
    (
        sys.executable,
        "scripts/run_m246_a010_lane_a_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m246-b014-lane-b-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m246-c024-lane-c-readiness",
    ),
    (
        sys.executable,
        D010_REAL_READINESS_SCRIPT,
    ),
)

FINAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py",
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

    print("[ok] M246-E013 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
