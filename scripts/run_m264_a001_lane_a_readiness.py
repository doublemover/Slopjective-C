#!/usr/bin/env python3
"""Readiness runner for M264-A001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M259-E002 + M263-E002 + M264-A001 (current versioning/conformance claims remain bounded by the previously validated bootstrap-complete runnable subset without rerunning the full bootstrap matrix on every M264 issue)")
    run(["python", "scripts/check_m264_a001_runnable_feature_claim_inventory_and_mode_truth_surface_contract_and_architecture_freeze.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_a001_runnable_feature_claim_inventory_and_mode_truth_surface_contract_and_architecture_freeze.py",
        "-q",
    ])
    print("[ok] M264-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
