#!/usr/bin/env python3
"""Readiness runner for M268-B003."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M268-B002 + M268-B003 (await placement is live and the remaining unsupported async topologies now fail closed before continuation lowering lands)")
    run(["python", "scripts/check_m268_b003_async_diagnostics_and_compatibility_completion_edge_case_and_compatibility_completion.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m268_b003_async_diagnostics_and_compatibility_completion_edge_case_and_compatibility_completion.py", "-q"])
    print("[ok] M268-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
