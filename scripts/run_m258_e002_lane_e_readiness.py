#!/usr/bin/env python3
"""Run M258-E002 lane-E readiness without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M258-A002 + M258-B002 + M258-C002 + M258-D002 + M258-E001 + M258-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m258-a002-lane-a-readiness"),
    (
        sys.executable,
        "scripts/check_m258_b002_imported_metadata_conformance_effect_and_dispatch_preservation_rules_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m258_b002_imported_metadata_conformance_effect_and_dispatch_preservation_rules_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation.py",
        "-q",
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
    (
        sys.executable,
        "scripts/check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (lane-E replays the frozen cross-module chain and then runs the live import/module execution matrix against the emitted two-image runtime path)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}", flush=True)
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
                flush=True,
            )
            return completed.returncode
    print("[ok] M258-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
