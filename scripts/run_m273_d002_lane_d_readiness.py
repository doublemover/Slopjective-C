#!/usr/bin/env python3
"""Run the issue-local M273-D002 readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M273-C003 + M273-D001 + M273-D002 (native-driver Part 10 host-process/cache integration now launches the frontend runner on cold cache misses, reuses deterministic cache entries on warm repeats, and preserves the contract across imported runtime surfaces and cross-module link plans)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation.py", "-q"])
    print("[ok] M273-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
