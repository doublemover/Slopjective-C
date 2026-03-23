#!/usr/bin/env python3
"""Readiness runner for M269-A002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M269-A001 + M269-A002 (task creation/task-group source sites are now published through one dedicated frontend packet before later runnable M269 lanes consume them)")
    run(["python", "scripts/check_m269_a002_frontend_task_group_and_cancellation_surface_completion_core_feature_implementation.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m269_a002_frontend_task_group_and_cancellation_surface_completion_core_feature_implementation.py", "-q"])
    print("[ok] M269-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
