#!/usr/bin/env python3
"""Lane-D readiness runner for M271-D002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M271-D001 + M271-C003 -> M271-D002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m271-d002-lane-d-readiness",
        "--summary-out",
        "tmp/reports/m271/M271-D002/ensure_objc3c_native_build_summary.json",
    ),
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        "scripts/check_m271_d002_live_cleanup_helpers_and_retainable_family_integration_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m271_d002_live_cleanup_helpers_and_retainable_family_integration_core_feature_implementation.py",
        "-q",
    ),
)


def main() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (lane D now proves linked Part 8 cleanup/resource execution and retainable-family helper traffic on one runnable path)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}", flush=True)
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M271-D002 lane-D readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
