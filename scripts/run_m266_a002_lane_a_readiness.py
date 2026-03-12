#!/usr/bin/env python3
"""Readiness runner for M266-A002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M266-A001 + M266-A002 (guard-condition lists and statement-form match grammar are now live while defer and advanced match forms remain fail-closed)")
    run(["python", "scripts/check_m266_a002_frontend_pattern_grammar_and_guard_surface_completion_core_feature_implementation.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m266_a002_frontend_pattern_grammar_and_guard_surface_completion_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M266-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
