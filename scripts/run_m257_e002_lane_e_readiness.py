#!/usr/bin/env python3
"""Run M257-E002 lane-E readiness without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M257-A002 + M257-B003 + M257-C003 + M257-D003 + M257-E001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m257-a002-lane-a-readiness"),
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
        "scripts/check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_d003_property_metadata_registration_and_reflective_access_helpers_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_e001_property_and_ivar_execution_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (lane-E replays the frozen property chain directly before the live runnable property/ivar matrix probe runs against the emitted object)",
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
    print("[ok] M257-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
