#!/usr/bin/env python3
"""Readiness runner for M267-A001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M266-E002 + M267-A001 (Part 6 source closure stays truthful while runnable propagation and catch semantics remain deferred to later M267 issues)")
    run([
        "python",
        "scripts/check_m267_a001_throws_try_do_catch_result_and_bridging_source_closure_contract_and_architecture_freeze.py",
    ])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m267_a001_throws_try_do_catch_result_and_bridging_source_closure_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M267-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
