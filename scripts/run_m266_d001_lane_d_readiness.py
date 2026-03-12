#!/usr/bin/env python3
"""Run the focused M266-D001 lane-D readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M266-C002 -> M266-D001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m266-d001-lane-d-readiness",
        "--summary-out",
        "tmp/reports/m266/M266-D001/ensure_objc3c_native_build_summary.json",
    ),
    (
        sys.executable,
        "scripts/check_m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m266_d001_cleanup_and_unwind_integration_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m266_d001_cleanup_and_unwind_integration_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (lane-D freeze stays pinned to the live M266-C002 cleanup insertion path while rerunning only the directly affected runtime/toolchain boundary)"
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M266-D001 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
