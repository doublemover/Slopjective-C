#!/usr/bin/env python3
"""Run M253-E001 lane-E readiness checks without nested npm recursion."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M253-A002 + M253-B003 + M253-C006 + M253-D003"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m253-a002-lane-a-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m253-b003-lane-b-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m253-c006-lane-c-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m253-d003-lane-d-readiness"),
    (sys.executable, "scripts/check_m253_e001_metadata_emission_gate.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m253_e001_metadata_emission_gate.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (matrix, object-format policy, binary inspection, and archive/static-link discovery gate)"
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
    print("[ok] M253-E001 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
