#!/usr/bin/env python3
"""Run M255-E001 lane-E readiness without nested npm recursion."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M255-A002 + M255-B003 + M255-C004 + M255-D004"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m255-a002-lane-a-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m255-b003-lane-b-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m255-c004-lane-c-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m255-d004-lane-d-readiness"),
    (sys.executable, "scripts/check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (taxonomy, legality, live lowering cutover, and live runtime resolution gate)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}", flush=True)
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
                flush=True,
            )
            return completed.returncode
    print("[ok] M255-E001 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
