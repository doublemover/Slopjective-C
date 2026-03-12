#!/usr/bin/env python3
"""Readiness runner for M265-A001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-E002 + M265-A001 (Part 3 type source closure stays truthful while runnable optional semantics remain deferred to later M265 issues)")
    run([
        "python",
        "scripts/check_m265_a001_optionals_nullability_pragmatic_generics_and_key_path_source_closure_contract_and_architecture_freeze.py",
    ])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m265_a001_optionals_nullability_pragmatic_generics_and_key_path_source_closure_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M265-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
