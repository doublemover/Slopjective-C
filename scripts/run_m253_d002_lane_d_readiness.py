#!/usr/bin/env python3
"""Run M253-D002 lane-D readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M253-D001 + M253-C004"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m253_d001_object_packaging_and_retention_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m253_d001_object_packaging_and_retention_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m253_d002_linker_retention_anchors_and_dead_strip_resistance_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m253_d002_linker_retention_anchors_and_dead_strip_resistance_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (metadata payload emission, object packaging, and single-library retention proof)"
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
    print("[ok] M253-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
