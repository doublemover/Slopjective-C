#!/usr/bin/env python3
"""Readiness runner for M270-A002."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M270-A001 + M270-A002 (actor/isolation source closure now widens into real actor-member frontend admission before lane-B semantic legality)")
    run(["python", "scripts/check_m270_a002_frontend_actor_member_and_isolation_annotation_surface_completion_core_feature_implementation.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m270_a002_frontend_actor_member_and_isolation_annotation_surface_completion_core_feature_implementation.py", "-q"])
    print("[ok] M270-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
