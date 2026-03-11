#!/usr/bin/env python3
"""Run the M263-B002 lane-B readiness chain without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"

DEPENDENCY_TOKEN = "M263-B001 + M263-A002 + M254-B002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m263-b002-lane-b-readiness",
        "--summary-out",
        "tmp/reports/m263/M263-B002/ensure_objc3c_native_build_summary.json",
    ),
    (
        sys.executable,
        "scripts/check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "scripts/check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "scripts/check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (M263-B002 preserves the emitted M263-A002 closure and the live M254-B002 semantics packet while landing the live duplicate/order bridge)"
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
    print("[ok] M263-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
