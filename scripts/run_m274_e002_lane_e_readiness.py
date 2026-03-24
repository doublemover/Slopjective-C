#!/usr/bin/env python3
"""Run M274-E002 lane-E closeout checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M274-A003 + M274-B004 + M274-C003 + M274-D002 + M274-E001 + M274-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (sys.executable, "scripts/run_m274_a003_lane_a_readiness.py"),
    (sys.executable, "scripts/run_m274_b004_lane_b_readiness.py"),
    (sys.executable, "scripts/run_m274_c003_lane_c_readiness.py"),
    (sys.executable, "scripts/run_m274_d002_lane_d_readiness.py"),
    (sys.executable, "scripts/run_m274_e001_lane_e_readiness.py"),
    (sys.executable, "scripts/check_m274_e002_cross_language_execution_matrix_cross_lane_integration_sync.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m274_e002_cross_language_execution_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (the lane-E closeout replays the published Part 11 foreign-surface, Swift-metadata, FFI-preservation, generated-bridge, and interop-gate proofs, then freezes one runnable cross-language matrix over C/C++ foreign-surface continuity, Swift metadata/isolation continuity, generated bridge artifact continuity, and cross-module runtime-import/link-plan continuity)",
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
    print("[ok] M274-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
