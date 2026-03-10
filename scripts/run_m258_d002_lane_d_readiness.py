#!/usr/bin/env python3
"""Run the M258-D002 lane-D readiness chain without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M258-C002 + M258-D001 + M258-D002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "scripts/check_m258_d001_cross_module_build_and_runtime_orchestration_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "scripts/check_m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (M258-D002 lands the real cross-module packaging, link-plan, linker-response, and runtime-registration happy path above D001 and C002)"
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
    print("[ok] M258-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
