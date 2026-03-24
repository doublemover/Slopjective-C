#!/usr/bin/env python3
"""Run M273-D001 lane-D readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> None:
    print("[info] dependency continuity token: M273-C003 + M273-D001 (cross-module replay preservation now freezes one truthful Part 10 host/runtime boundary over the existing private property runtime slice while macro host execution and package loading remain fail-closed until M273-D002)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m273_d001_expansion_host_and_runtime_boundary_contract_and_architecture_freeze.py"])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m273_d001_expansion_host_and_runtime_boundary_contract_and_architecture_freeze.py",
        "-q",
    ])


if __name__ == "__main__":
    main()
