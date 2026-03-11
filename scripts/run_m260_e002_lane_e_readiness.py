#!/usr/bin/env python3
"""Run the M260-E002 closeout readiness chain."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M260-A002 + M260-B003 + M260-C002 + M260-D002 + M260-E001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "scripts/check_m260_e001_ownership_runtime_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "scripts/check_m260_e002_runnable_ownership_smoke_matrix_and_docs_cross_lane_integration_sync.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m260_e002_runnable_ownership_smoke_matrix_and_docs_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (lane-E closeout aligns the emitted ownership surface, semantic guardrails, runtime hooks, runtime implementation, and frozen gate before M260 closes)",
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
    print("[ok] M260-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
