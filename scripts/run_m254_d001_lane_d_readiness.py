#!/usr/bin/env python3
"""Run M254-D001 lane-D readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M254-A002 + M254-B001 + M254-C003"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m254_b001_bootstrap_invariants_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m254_b001_bootstrap_invariants_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m254_c003_registration_table_emission_and_image_local_initialization_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m254_c003_registration_table_emission_and_image_local_initialization_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (registration manifest, bootstrap invariants, and registration table emission must remain synchronized with the bootstrap API freeze)"
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
    print("[ok] M254-D001 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
