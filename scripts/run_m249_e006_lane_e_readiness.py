#!/usr/bin/env python3
"""Run M249-E006 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-e005-lane-e-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-a006-lane-a-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-b006-lane-b-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-c006-lane-c-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m249_d005_lane_d_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m249_e006_lane_e_release_gate_docs_and_runbooks_edge_case_expansion_and_robustness_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m249_e006_lane_e_release_gate_docs_and_runbooks_edge_case_expansion_and_robustness_contract.py",
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
    print("[ok] M249-E006 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
