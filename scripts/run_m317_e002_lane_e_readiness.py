#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M317-A001 + M317-A002 + M317-B001 + M317-B002 + M317-B003 + M317-C001 + M317-C002 + M317-D001 + M317-E001(summary replay) + M317-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/run_m317_a001_lane_a_readiness.py"),
    (sys.executable, "scripts/run_m317_a002_lane_a_readiness.py"),
    (sys.executable, "scripts/run_m317_b001_lane_b_readiness.py"),
    (sys.executable, "scripts/run_m317_b002_lane_b_readiness.py"),
    (sys.executable, "scripts/run_m317_b003_lane_b_readiness.py"),
    (sys.executable, "scripts/run_m317_c001_lane_c_readiness.py"),
    (sys.executable, "scripts/run_m317_c002_lane_c_readiness.py"),
    (sys.executable, "scripts/run_m317_d001_lane_d_readiness.py"),
    (sys.executable, "scripts/check_m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync.py"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (the M317 closeout replays the historical E001 gate through its published summary, rather than rerunning that pre-closeout gate after #7833 is closed, and then hands the cleanup-first sequence forward to M313 without widening the planning surface)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}", flush=True)
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {command_text}", file=sys.stderr, flush=True)
            return completed.returncode
    print("[ok] M317-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
