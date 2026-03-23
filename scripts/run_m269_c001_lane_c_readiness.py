#!/usr/bin/env python3
"""Readiness runner for M269-C001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M269-B003 + M269-C001 (Part 7 task semantics are live and task runtime lowering is now frozen as an emitted contract)")
    run(["python", "scripts/check_m269_c001_task_runtime_lowering_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m269_c001_task_runtime_lowering_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M269-C001 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
