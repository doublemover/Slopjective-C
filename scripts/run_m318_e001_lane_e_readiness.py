#!/usr/bin/env python3
"""Readiness runner for M318-E001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    commands = [
        [sys.executable, "scripts/check_m318_e001_sustainable_progress_gate_contract_and_architecture_freeze.py"],
        [sys.executable, "-m", "pytest", "tests/tooling/test_check_m318_e001_sustainable_progress_gate_contract_and_architecture_freeze.py", "-q"],
    ]
    for command in commands:
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            return completed.returncode
    print("M318-E001 lane-E readiness passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
