#!/usr/bin/env python3
"""Readiness runner for M269-B003."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M269-B002 + M269-B003 (structured-task legality now carries explicit executor-affinity and detached-hop fail-closed semantics while runnable lowering remains later M269 work)")
    run(["python", "scripts/check_m269_b003_executor_hop_and_affinity_completion_edge_case_and_compatibility_completion.py"])
    run(["python", "-m", "pytest", "tests/tooling/test_check_m269_b003_executor_hop_and_affinity_completion_edge_case_and_compatibility_completion.py", "-q"])
    print("[ok] M269-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
