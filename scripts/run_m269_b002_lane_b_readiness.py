#!/usr/bin/env python3
"""Readiness runner for M269-B002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M269-B001 + M269-B002 (task/executor/cancellation semantic model now drives live structured-task fail-closed diagnostics while runnable lowering and scheduler runtime remain later M269 work)")
    run(["python", "scripts/check_m269_b002_structured_task_and_cancellation_legality_semantics_core_feature_implementation.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m269_b002_structured_task_and_cancellation_legality_semantics_core_feature_implementation.py", "-q"])
    print("[ok] M269-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
