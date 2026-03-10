#!/usr/bin/env python3
"""Run the M259-A002 lane-A readiness chain."""

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
        "[info] dependency continuity token: M259-A001 + M259-A002 "
        "(lane A widens the frozen runnable sample surface into one integrated live object/category/protocol/property proof before M259-B001)"
    )
    run([sys.executable, "scripts/check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py", "-q"])
    run([sys.executable, "scripts/check_m259_a002_canonical_runnable_sample_set.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m259_a002_canonical_runnable_sample_set.py", "-q"])
    print("[ok] M259-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
