#!/usr/bin/env python3
"""Lane-B readiness runner for M266-B001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = [
    [sys.executable, "scripts/check_m266_b001_control_flow_and_pattern_semantic_model_contract_and_architecture_freeze.py"],
    [sys.executable, "-m", "pytest", "tests/tooling/test_check_m266_b001_control_flow_and_pattern_semantic_model_contract_and_architecture_freeze.py", "-q"],
]


def main() -> int:
    for command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            return completed.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
