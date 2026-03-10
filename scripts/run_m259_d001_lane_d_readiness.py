#!/usr/bin/env python3
"""Run the M259-D001 lane-D readiness chain."""

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
        "[info] dependency continuity token: M259-C002 + M259-D001 "
        "(lane D freezes the supported local build/compile/smoke/replay/package boundary before M259-D002 implements workflow and packaging expansion)"
    )
    run([sys.executable, "scripts/check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py"])
    run(
        [
            sys.executable,
            "-m",
            "pytest",
            "tests/tooling/test_check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py",
            "-q",
        ]
    )
    print("[ok] M259-D001 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
