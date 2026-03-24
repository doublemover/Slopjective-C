#!/usr/bin/env python3
"""Run M273-E002 lane-E closeout checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M273-A003 + M273-B004 + M273-C003 + M273-D002 + M273-E001 + M273-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (sys.executable, "scripts/run_m273_a003_lane_a_readiness.py"),
    (sys.executable, "scripts/run_m273_b004_lane_b_readiness.py"),
    (sys.executable, "scripts/run_m273_c003_lane_c_readiness.py"),
    (sys.executable, "scripts/run_m273_d002_lane_d_readiness.py"),
    (sys.executable, "scripts/run_m273_e001_lane_e_readiness.py"),
    (sys.executable, "scripts/check_m273_e002_runnable_expansion_and_behavior_matrix_cross_lane_integration_sync.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m273_e002_runnable_expansion_and_behavior_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (the lane-E closeout replays the published Part 10 source, legality, replay-preservation, host-process/cache, and conformance-gate proofs, then freezes one runnable metaprogramming matrix over derive continuity, macro package/provenance plus host-cache continuity, property-behavior legality/replay continuity, and cross-module preservation)",
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
    print("[ok] M273-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
