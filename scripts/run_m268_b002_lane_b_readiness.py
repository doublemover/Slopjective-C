#!/usr/bin/env python3
"""Readiness runner for M268-B002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M268-B001 + M268-B002 (async legality packet now feeds live await placement enforcement while runnable async lowering remains later M268 work)")
    run(["python", "scripts/check_m268_b002_await_suspension_and_resume_semantics_core_feature_implementation.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m268_b002_await_suspension_and_resume_semantics_core_feature_implementation.py", "-q"])
    print("[ok] M268-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
