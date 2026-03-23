#!/usr/bin/env python3
"""Readiness runner for M269-B001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M269-A002 + M269-B001 (task-group/cancellation source closure now feeds a live semantic legality packet while runnable lowering and scheduler runtime remain later M269 work)")
    run(["python", "scripts/check_m269_b001_task_executor_and_cancellation_semantic_model_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m269_b001_task_executor_and_cancellation_semantic_model_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M269-B001 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
