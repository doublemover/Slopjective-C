#!/usr/bin/env python3
"""Run M256-E002 lane-E readiness without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M256-A003 + M256-B004 + M256-C003 + M256-D004 + M256-E001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "scripts/check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (source closure, inheritance legality, realization records, canonical runnable runtime proof, and the frozen E001 conformance gate are replayed directly before the live E002 execution matrix runs, without recursively rebuilding the entire lane-A stack)",
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
    print("[ok] M256-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
