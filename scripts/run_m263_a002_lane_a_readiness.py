#!/usr/bin/env python3
"""Run the M263-A002 lane-A readiness chain without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"

DEPENDENCY_TOKEN = "M263-A001 + M254-A002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m263-a002-lane-a-readiness",
        "--summary-out",
        "tmp/reports/m263/M263-A002/ensure_objc3c_native_build_summary.json",
    ),
    (
        sys.executable,
        "scripts/check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py",
    ),
    (
        sys.executable,
        "scripts/check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (M263-A002 extends M263-A001 into emitted module.runtime-registration-descriptor.json artifacts)"
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
    print("[ok] M263-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
