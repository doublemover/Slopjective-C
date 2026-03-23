#!/usr/bin/env python3
"""Readiness runner for M270-C001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M270-B003 + M270-C001 (actor legality and hazard packets now feed one deterministic lowering contract while live thunk bodies remain later lane-C work)")
    run(["python", "scripts/check_m270_c001_actor_lowering_and_metadata_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m270_c001_actor_lowering_and_metadata_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M270-C001 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
