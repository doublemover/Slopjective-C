#!/usr/bin/env python3
"""Run M269-C002 lane-C readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M269-C001 + M269-C002 (task-runtime lowering freeze now feeds live private helper-backed spawn/hop/cancel rewrites while broader task-group ABI completion remains later M269 work)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m269_c002_executor_hop_cancellation_and_task_spawning_lowering_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m269_c002_executor_hop_cancellation_and_task_spawning_lowering_core_feature_implementation.py", "-q"])


if __name__ == "__main__":
    main()
