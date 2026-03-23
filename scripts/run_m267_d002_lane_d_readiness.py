#!/usr/bin/env python3
"""Lane-D readiness runner for M267-D002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M267-C002 + M267-C003 + M267-D001 -> M267-D002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m267-d002-lane-d-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-D002/ensure_objc3c_native_build_summary.json",
    ),
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        "scripts/check_m267_d001_error_runtime_and_bridge_helper_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation.py",
        "-q",
    ),
)


def main() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (lane D now proves the Part 6 status-bridge plus NSError catch path executes through the packaged runtime and the private helper ABI from D001; C002/C003 remain consumed by the live D002 object probe rather than being revalidated through stale historical next_issue markers)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}", flush=True)
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M267-D002 lane-D readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
