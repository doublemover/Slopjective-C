#!/usr/bin/env python3
"""Run M263-E002 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M263-E001 + M263-C003 + M263-D003 + M263-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m263-e001-lane-e-readiness"),
    (sys.executable, "scripts/check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (bootstrap completion gate, archive replay proof, live restart hardening, and the published bootstrap matrix must stay synchronized)"
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
    print("[ok] M263-E002 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
