#!/usr/bin/env python3
"""Run M255-A001 lane-A readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M255-A001"

CHECK_SCRIPT = "check:objc3c:m255-a001-dispatch-surface-classification-contract-and-architecture-freeze"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (sys.executable, "scripts/check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (dispatch taxonomy freeze must stay synchronized with the live runtime family)")
    print(f"[info] canonical checker script: {CHECK_SCRIPT}")
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {command_text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M255-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
