#!/usr/bin/env python3
"""Run M271-E002 lane-E closeout checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M271-A003 + M271-B004 + M271-C003 + M271-D002 + M271-E001 + M271-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (sys.executable, "scripts/run_m271_a003_lane_a_readiness.py"),
    (sys.executable, "scripts/run_m271_b004_lane_b_readiness.py"),
    (sys.executable, "scripts/run_m271_c003_lane_c_readiness.py"),
    (sys.executable, "scripts/run_m271_d002_lane_d_readiness.py"),
    (sys.executable, "scripts/run_m271_e001_lane_e_readiness.py"),
    (sys.executable, "scripts/check_m271_e002_runnable_system_extension_matrix_cross_lane_integration_sync.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m271_e002_runnable_system_extension_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (the lane-E closeout replays the published M271 source, semantic, lowering, runtime, and gate proofs, then freezes the runnable Part 8 matrix without widening the deferred borrowed-lifetime/runtime claims)",
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
    print("[ok] M271-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
