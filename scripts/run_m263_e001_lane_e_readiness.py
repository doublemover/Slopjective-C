#!/usr/bin/env python3
"""Run M263-E001 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M263-A002 + M263-B003 + M263-C003 + M263-D003 + M263-E001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m263-a002-lane-a-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m263-b003-lane-b-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m263-c003-lane-c-readiness"),
    (NPM_EXECUTABLE, "run", "check:objc3c:m263-d003-lane-d-readiness"),
    (
        sys.executable,
        "scripts/check_m263_e001_bootstrap_completion_conformance_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m263_e001_bootstrap_completion_conformance_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (bootstrap completion evidence must stay synchronized across emitted descriptor authority, restart semantics, archive/static-link replay, and live restart hardening)"
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False, cwd=str(ROOT))
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M263-E001 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
