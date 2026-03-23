#!/usr/bin/env python3
"""Run M270-D001 lane-D readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M270-C002 + M270-D001 (the live actor helper/runtime slice now freezes one canonical private actor-state and executor-binding contract while broader mailbox runtime realization remains later M270 work)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m270_d001_actor_runtime_and_executor_binding_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m270_d001_actor_runtime_and_executor_binding_contract_and_architecture_freeze.py", "-q"])


if __name__ == "__main__":
    main()
