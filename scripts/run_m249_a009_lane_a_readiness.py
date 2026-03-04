#!/usr/bin/env python3
"""Run M249-A009 lane-A readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-a008-lane-a-readiness",
    ),
    (
        sys.executable,
        "scripts/check_m249_a009_feature_packaging_surface_and_compatibility_contracts_integration_closeout_and_gate_signoff_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m249_a009_feature_packaging_surface_and_compatibility_contracts_integration_closeout_and_gate_signoff_contract.py",
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
    print("[ok] M249-A009 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())

