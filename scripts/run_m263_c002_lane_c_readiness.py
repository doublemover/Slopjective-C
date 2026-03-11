#!/usr/bin/env python3
"""Run the M263-C002 lane-C readiness chain without npm recursion."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"

DEPENDENCY_TOKEN = "M263-A002 + M263-B003 + M263-C001"
COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m263-c002-lane-c-readiness",
        "--summary-out",
        "tmp/reports/m263/M263-C002/ensure_objc3c_native_build_summary.json",
    ),
    (
        sys.executable,
        "scripts/check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "scripts/check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py",
    ),
    (
        sys.executable,
        "scripts/check_m263_c001_constructor_root_and_init_array_lowering_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "scripts/check_m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (M263-C002 preserves the emitted A002 descriptor/image-root identities, the live B003 bootstrap semantics, and the frozen C001 ctor-root/table boundary while adding first-class registration-descriptor/image-root globals)"
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {command_text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M263-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
