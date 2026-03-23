#!/usr/bin/env python3
"""Run M269-D002 lane-D readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M269-C002 + M269-C003 + M269-D001 + M269-D002 (helper-backed Part 7 task runtime is now a live private execution boundary while broader cancellation/autorelease hardening remains M269-D003)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m269_d002_live_task_runtime_and_executor_implementation_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m269_d002_live_task_runtime_and_executor_implementation_core_feature_implementation.py", "-q"])


if __name__ == "__main__":
    main()
