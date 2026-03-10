#!/usr/bin/env python3
"""Run the M257-D002 lane-D readiness chain without recursive npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M257-A001..A002 + M257-B001..B003 + M257-C001..C003 + M257-D001 + M257-D002"

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
    (
        sys.executable,
        "scripts/check_m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_d001_runtime_property_and_layout_consumption_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_d001_runtime_property_and_layout_consumption_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_d002_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_d002_instance_allocation_layout_and_ivar_offset_runtime_support_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (source closure, property sema, lane-C lowering, the historical D001 freeze, and true D002 instance allocation must stay aligned without recursive issue runners)"
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
    print("[ok] M257-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())

