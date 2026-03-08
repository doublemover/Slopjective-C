#!/usr/bin/env python3
"""Run M254-E001 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M254-A002 + M254-B002 + M254-C003 + M254-D003 + M254-D004 + M254-E001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m254-a002-lane-a-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m254-b002-lane-b-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m254-c003-lane-c-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m254-d003-lane-d-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m254-d004-lane-d-readiness"),
    (sys.executable, "scripts/check_m254_e001_startup_registration_gate.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m254_e001_startup_registration_gate.py", "-q"),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (startup-registration, replay, and operator launch evidence must stay synchronized)"
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
    print("[ok] M254-E001 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
