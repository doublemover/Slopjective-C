#!/usr/bin/env python3
"""Run M247-C005 lane-C readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from shutil import which
from typing import Sequence


DEPENDENCY_TOKEN = "M247-C004"
NPM_COMMAND = which("npm") or which("npm.cmd") or "npm.cmd"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        NPM_COMMAND,
        "run",
        "check:objc3c:m247-c004-lane-c-readiness",
    ),
    (
        sys.executable,
        "scripts/check_m247_c005_lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m247_c005_lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (inherits lane-C core feature expansion readiness)"
    )
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
    print("[ok] M247-C005 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
