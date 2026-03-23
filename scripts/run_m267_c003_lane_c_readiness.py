#!/usr/bin/env python3
"""Lane-C readiness runner for M267-C003."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M267-B002 + M267-B003 + M267-C002 + M267-C003"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m267-c003-lane-c-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-C003/ensure_objc3c_native_build_summary.json",
    ),
    (
        sys.executable,
        "scripts/check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} "
        "M267-C003 result and bridging artifact replay completion"
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M267-C003 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
