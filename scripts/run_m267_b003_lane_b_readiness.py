#!/usr/bin/env python3
"""Lane-B readiness runner for M267-B003."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEPENDENCY_TOKEN = "M267-B002 + M267-B003"


def run(cmd: list[str]) -> None:
    completed = subprocess.run(cmd, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print(f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (Part 6 now enforces NSError/status bridge legality and only semantically valid bridge callables qualify for try)")
    run([sys.executable, "scripts/check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py", "-q"])
    print("[ok] M267-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
