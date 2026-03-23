#!/usr/bin/env python3
"""Run M269-D001 lane-D readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M269-C002 + M269-C003 + M269-D001 (helper-backed Part 7 task runtime now freezes one private scheduler/executor contract while live runtime implementation remains M269-D002)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m269_d001_scheduler_and_executor_runtime_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m269_d001_scheduler_and_executor_runtime_contract_and_architecture_freeze.py", "-q"])


if __name__ == "__main__":
    main()
