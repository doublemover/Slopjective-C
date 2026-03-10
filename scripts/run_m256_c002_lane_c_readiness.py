#!/usr/bin/env python3
"""Run the M256-C002 lane-C readiness chain without recursive npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M256-B004 + M256-C001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m256-b004-lane-b-readiness"),
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (realized-object legality plus the frozen C001 artifact boundary remain aligned while C002 proves live executable body binding)"
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M256-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
