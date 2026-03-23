#!/usr/bin/env python3
"""Readiness runner for M271-A001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M262-E002 + M266-E002 + M271-A001 (Part 8 source closure now admits resource locals, borrowed qualifiers, and explicit block capture lists before later M271 legality/lowering work)")
    run(["python", "scripts/check_m271_a001_resource_borrowed_pointer_and_capture_list_source_closure_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m271_a001_resource_borrowed_pointer_and_capture_list_source_closure_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M271-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
