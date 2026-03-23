#!/usr/bin/env python3
"""Run M269-C003 lane-C readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M269-C002 + M269-C003 (helper-backed Part 7 task-runtime lowering now publishes a dedicated ABI/artifact packet while broader scheduler freeze work remains later M269 work)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m269_c003_task_group_and_runtime_abi_completion_core_feature_expansion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m269_c003_task_group_and_runtime_abi_completion_core_feature_expansion.py", "-q"])


if __name__ == "__main__":
    main()

