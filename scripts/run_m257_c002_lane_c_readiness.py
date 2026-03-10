#!/usr/bin/env python3
"""Run the M257-C002 lane-C readiness chain without recursive npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M257-A001..A002 + M257-B001..B003 + M257-C001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_c001_accessor_and_layout_lowering_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_c001_accessor_and_layout_lowering_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (lane-C ivar layout emission stays aligned with the M257 source/sema/property lowering chain without recursive npm nesting)"
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
    print("[ok] M257-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
