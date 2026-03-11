#!/usr/bin/env python3
"""Run the M263-B003 lane-B readiness chain without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"

DEPENDENCY_TOKEN = "M263-B002 + M254-B002 + M254-D003"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m263-b003-lane-b-readiness",
        "--summary-out",
        "tmp/reports/m263/M263-B003/ensure_objc3c_native_build_summary.json",
    ),
    (
        sys.executable,
        "scripts/check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "scripts/check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "scripts/check_m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "scripts/check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (M263-B003 preserves the live M263-B002 legality bridge and the M254 bootstrap/reset runtime contracts while landing deterministic restart semantics)"
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M263-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
