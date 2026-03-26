#!/usr/bin/env python3
"""Shared-harness-first readiness runner for M318-A001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

COMMANDS = [
    [sys.executable, str(ROOT / "scripts" / "check_m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze.py")],
    [sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze.py"), "-q"],
]


def main() -> int:
    for command in COMMANDS:
        result = subprocess.run(command, cwd=ROOT)
        if result.returncode != 0:
            return result.returncode
    print("M318-A001 lane-A readiness passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
