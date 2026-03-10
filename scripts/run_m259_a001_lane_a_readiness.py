#!/usr/bin/env python3
"""Run the M259-A001 lane-A readiness chain."""

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
        "[info] dependency continuity token: M256-D004 + M257-E002 + M258-E002 + M259-A001 "
        "(lane-A freezes the truthful runnable sample surface before A002 widens the canonical sample set)"
    )
    run([sys.executable, "scripts/check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M259-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
