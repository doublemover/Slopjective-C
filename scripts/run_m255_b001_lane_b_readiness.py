#!/usr/bin/env python3
"""Run M255-B001 lane-B readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M255-B001"

CHECK_SCRIPT = "check:objc3c:m255-b001-dispatch-legality-and-selector-resolution-contract-and-architecture-freeze"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m255_b001_dispatch_legality_and_selector_resolution_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m255_b001_dispatch_legality_and_selector_resolution_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (selector legality and fail-closed normalization "
        "must stay synchronized with the live dispatch path)"
    )
    print(f"[info] canonical checker script: {CHECK_SCRIPT}")
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: "
                f"{command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M255-B001 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
