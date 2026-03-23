#!/usr/bin/env python3
"""Readiness runner for M268-C001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M268-B003 + M268-C001 (Part 7 async semantics are live and continuation/await lowering handoff is now frozen as an emitted contract)")
    run(["python", "scripts/check_m268_c001_continuation_abi_and_async_lowering_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m268_c001_continuation_abi_and_async_lowering_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M268-C001 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
