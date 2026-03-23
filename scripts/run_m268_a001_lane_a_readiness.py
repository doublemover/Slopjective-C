#!/usr/bin/env python3
"""Readiness runner for M268-A001."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M267-E002 + M268-A001 (async syntax is now parser-owned while continuation ABI and runtime execution remain later M268 work)")
    run(["python", "scripts/check_m268_a001_async_await_and_executor_annotation_source_closure_contract_and_architecture_freeze.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m268_a001_async_await_and_executor_annotation_source_closure_contract_and_architecture_freeze.py", "-q"])
    print("[ok] M268-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
