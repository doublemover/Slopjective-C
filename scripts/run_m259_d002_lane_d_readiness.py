#!/usr/bin/env python3
"""Run the M259-D002 lane-D readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    print(f"[run] {' '.join(command)}")
    completed = subprocess.run(command, cwd=str(ROOT), check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print(
        "[info] dependency continuity token: M259-D001 + M259-D002 "
        "(lane D expands the frozen runnable-core operations boundary into a staged package workflow and validates the bundled compile/smoke/replay path before M259-D003 documents bring-up)"
    )
    run([sys.executable, "scripts/run_m259_d001_lane_d_readiness.py"])
    run(
        [
            sys.executable,
            "-m",
            "pytest",
            "tests/tooling/test_check_m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation.py",
            "-q",
        ]
    )
    run([sys.executable, "scripts/check_m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation.py"])
    print("[ok] M259-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
