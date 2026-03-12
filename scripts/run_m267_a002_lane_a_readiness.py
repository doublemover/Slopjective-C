#!/usr/bin/env python3
"""Readiness runner for M267-A002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M267-A001 + M267-A002 (Part 6 source closure now includes canonical bridge markers while runnable error propagation remains deferred)")
    run([
        "python",
        "scripts/check_m267_a002_frontend_nserror_and_status_bridging_surface_completion_core_feature_implementation.py",
    ])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m267_a002_frontend_nserror_and_status_bridging_surface_completion_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M267-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
