#!/usr/bin/env python3
"""Readiness runner for M268-C003."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M268-B003 + M268-C002 + M268-C003 (Part 7 now proves the supported non-suspending async slice composes with autoreleasepool scope hooks and defer cleanup emission)")
    run(["python", "scripts/check_m268_c003_suspension_autorelease_and_cleanup_integration_core_feature_expansion.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m268_c003_suspension_autorelease_and_cleanup_integration_core_feature_expansion.py", "-q"])
    print("[ok] M268-C003 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
