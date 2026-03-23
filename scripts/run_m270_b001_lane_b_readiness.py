#!/usr/bin/env python3
"""Readiness runner for M270-B001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M270-A002 + M270-B001 (actor-member source closure now feeds one dedicated actor/sendability sema packet before later legality/runtime work)")
    run(["python", "scripts/check_m270_b001_isolation_and_sendable_semantic_model_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m270_b001_isolation_and_sendable_semantic_model_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M270-B001 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
