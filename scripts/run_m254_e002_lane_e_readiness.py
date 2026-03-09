#!/usr/bin/env python3
"""Run M254-E002 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M254-E001 + M254-D004 + M254-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m254-e001-lane-e-readiness"),
    (sys.executable, "scripts/check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (startup gate, launch contract, and published operator runbook must stay synchronized)"
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
    print("[ok] M254-E002 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
