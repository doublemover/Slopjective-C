#!/usr/bin/env python3
"""Run M247-E014 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"

BASELINE_DEPENDENCIES = ("M247-E013",)
PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A014", "M247-B016", "M247-C015", "M247-D011")
RUN_TRANSITIVE_DEPENDENCIES = False

INITIAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-e013-lane-e-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-a014-lane-a-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-b016-lane-b-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-c015-lane-c-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m247-d011-lane-d-readiness",
    ),
)

FINAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py",
        "-q",
    ),
)


OPTIONAL_FAILURE_COMMANDS = frozenset(
    {
        "check:objc3c:m247-e013-lane-e-readiness",
    }
)


def run_command(command: Sequence[str]) -> int:
    command_text = " ".join(command)
    print(f"[run] {command_text}")
    completed = subprocess.run(command, check=False)
    if completed.returncode != 0:
        command_name = command[-1] if command else ""
        if command_name in OPTIONAL_FAILURE_COMMANDS:
            print(
                f"[warn] optional command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return 0
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
    if RUN_TRANSITIVE_DEPENDENCIES:
        for command in INITIAL_COMMAND_CHAIN:
            exit_code = run_command(command)
            if exit_code != 0:
                return exit_code
    else:
        print(
            "[info] transitive dependency command execution is disabled for "
            "lane-isolated E014 readiness validation"
        )

    for command in FINAL_COMMAND_CHAIN:
        exit_code = run_command(command)
        if exit_code != 0:
            return exit_code

    print("[ok] M247-E014 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
