#!/usr/bin/env python3
"""Run the focused M262-D001 lane-D readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M262-C004 -> M262-D001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m262-d001-lane-d-readiness",
        "--summary-out",
        "tmp/reports/m262/M262-D001/ensure_objc3c_native_build_summary.json",
    ),
    (
        sys.executable,
        "scripts/check_m262_c004_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (runtime ARC helper freeze stays aligned with the live C004 lowering boundary while only rerunning the directly affected lane-D stack)"
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M262-D001 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
