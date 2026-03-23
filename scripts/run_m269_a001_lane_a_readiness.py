#!/usr/bin/env python3
"""Readiness runner for M269-A001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M268-E002 + M269-A001 (async source closure now hands off the task/executor/cancellation source boundary to the later runnable M269 lanes)")
    run(["python", "scripts/check_m269_a001_task_executor_and_cancellation_source_closure_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m269_a001_task_executor_and_cancellation_source_closure_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M269-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
