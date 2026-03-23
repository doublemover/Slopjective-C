#!/usr/bin/env python3
"""Run M269-E002 lane-E closeout checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M269-A002 + M269-B003 + M269-C003 + M269-D003 + M269-E001 + M269-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (sys.executable, "scripts/run_m269_a002_lane_a_readiness.py"),
    (sys.executable, "scripts/run_m269_b003_lane_b_readiness.py"),
    (sys.executable, "scripts/run_m269_c003_lane_c_readiness.py"),
    (sys.executable, "scripts/run_m269_d003_lane_d_readiness.py"),
    (sys.executable, "scripts/run_m269_e001_lane_e_readiness.py"),
    (sys.executable, "scripts/check_m269_e002_runnable_task_and_executor_matrix_cross_lane_integration_sync.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m269_e002_runnable_task_and_executor_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (the lane-E closeout replays the published M269 source, semantic, lowering, runtime-helper, hardening, and gate proofs, then freezes the runnable task/executor matrix without widening the Part 7 runtime claim)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}", flush=True)
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
                flush=True,
            )
            return completed.returncode
    print("[ok] M269-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
