#!/usr/bin/env python3
"""Readiness runner for M265-A002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M265-A001 + M265-A002 (Part 3 lane-A now admits parser-owned optional bindings, sends, optional-member-access sugar, nil-coalescing, and typed key paths)")
    run([
        "python",
        "scripts/check_m265_a002_frontend_support_for_optional_sends_binds_coalescing_and_typed_key_paths_core_feature_implementation.py",
    ])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m265_a002_frontend_support_for_optional_sends_binds_coalescing_and_typed_key_paths_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M265-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
