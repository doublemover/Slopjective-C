#!/usr/bin/env python3
"""Readiness runner for M264-B003."""

from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-B002 + M264-B003 (compatibility claim truth remains fail-closed while canonical-interface claims stay equivalent-only and macro claims stay suppressed)")
    run(["python", "scripts/run_m264_b002_lane_b_readiness.py"])
    run(["python", "scripts/check_m264_b003_canonical_interface_and_feature_macro_truthfulness_edge_case_and_compatibility_completion.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_b003_canonical_interface_and_feature_macro_truthfulness_edge_case_and_compatibility_completion.py",
        "-q",
    ])
    print("[ok] M264-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
