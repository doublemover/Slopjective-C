#!/usr/bin/env python3
"""Direct readiness runner for M254-B002."""

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
PYTHON = sys.executable
NPM = shutil.which("npm.cmd") or shutil.which("npm")


def run_step(command: Sequence[str]) -> int:
    completed = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    return completed.returncode


def main() -> int:
    if not PYTHON:
        raise RuntimeError("python interpreter is unavailable")
    if not NPM:
        raise RuntimeError("npm executable is unavailable")
    steps = (
        [NPM, "run", "build:objc3c-native"],
        [
            PYTHON,
            "scripts/check_m254_b001_bootstrap_invariants_contract.py",
            "--skip-dynamic-probes",
        ],
        [
            PYTHON,
            "scripts/check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py",
        ],
        [
            PYTHON,
            "-m",
            "pytest",
            "tests/tooling/test_check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py",
            "-q",
        ],
    )
    for command in steps:
        if run_step(command) != 0:
            return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
