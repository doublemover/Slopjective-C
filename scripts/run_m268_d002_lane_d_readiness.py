#!/usr/bin/env python3
"""Lane-D readiness runner for M268-D002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M268-D001 + M268-C003 -> M268-D002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m268-d002-lane-d-readiness",
        "--summary-out",
        "tmp/reports/m268/M268-D002/ensure_objc3c_native_build_summary.json",
    ),
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        "scripts/check_m268_d002_live_continuation_and_scheduler_boundary_implementation_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m268_d002_live_continuation_and_scheduler_boundary_implementation_core_feature_implementation.py",
        "-q",
    ),
)


def main() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (lane D now proves the supported direct-call await path executes through the private continuation helper cluster)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}", flush=True)
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M268-D002 lane-D readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
