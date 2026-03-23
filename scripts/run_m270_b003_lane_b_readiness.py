#!/usr/bin/env python3
"""Readiness runner for M270-B003."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M270-B002 + M270-B003 (actor-method hazard and escape diagnostics now fail closed while runnable actor runtime remains later M270 work)")
    run(["python", "scripts/check_m270_b003_data_race_hazard_and_escape_diagnostic_completion_edge_case_and_compatibility_completion.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m270_b003_data_race_hazard_and_escape_diagnostic_completion_edge_case_and_compatibility_completion.py", "-q"])
    print("[ok] M270-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
