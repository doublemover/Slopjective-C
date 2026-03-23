#!/usr/bin/env python3
"""Readiness runner for M270-A001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M269-E002 + M270-A001 (task/executor completion now hands off the truthful actor/isolation/sendability source boundary to later M270 semantic and runtime work)")
    run(["python", "scripts/check_m270_a001_actor_isolation_and_sendable_source_closure_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m270_a001_actor_isolation_and_sendable_source_closure_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M270-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
