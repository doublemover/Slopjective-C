#!/usr/bin/env python3
"""Readiness runner for M266-A001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M265-E002 + M266-A001 (Part 5 source closure stays truthful while runnable defer/match semantics remain deferred to later M266 issues)")
    run([
        "python",
        "scripts/check_m266_a001_defer_guard_match_and_pattern_source_closure_contract_and_architecture_freeze.py",
    ])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m266_a001_defer_guard_match_and_pattern_source_closure_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M266-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
