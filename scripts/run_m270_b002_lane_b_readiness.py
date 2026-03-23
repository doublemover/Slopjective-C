#!/usr/bin/env python3
"""Readiness runner for M270-B002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M270-B001 + M270-B002 (actor/sendability freeze packet now drives live actor-method fail-closed diagnostics while runnable actor runtime remains later M270 work)")
    run(["python", "scripts/check_m270_b002_actor_isolation_and_sendability_enforcement_core_feature_implementation.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m270_b002_actor_isolation_and_sendability_enforcement_core_feature_implementation.py", "-q"])
    print("[ok] M270-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
