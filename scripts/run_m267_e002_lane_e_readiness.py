#!/usr/bin/env python3
"""Run M267-E002 lane-E closeout checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M267-A001 + M267-A002 + M267-B001 + M267-B002 + M267-B003 + M267-C001 + M267-C002 + M267-C003 + M267-D001 + M267-D002 + M267-D003 + M267-E001 + M267-E002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        "scripts/check_m267_a001_throws_try_do_catch_result_and_bridging_source_closure_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (sys.executable, "scripts/run_m267_a002_lane_a_readiness.py"),
    (
        sys.executable,
        "scripts/check_m267_b001_error_carrier_and_propagation_semantic_model_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m267_b002_try_do_catch_and_propagation_semantics_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m267_c001_throws_abi_and_propagation_lowering_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (sys.executable, "scripts/run_m267_c003_lane_c_readiness.py"),
    (
        sys.executable,
        "scripts/check_m267_d001_error_runtime_and_bridge_helper_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (sys.executable, "scripts/run_m267_d003_lane_d_readiness.py"),
    (sys.executable, "scripts/run_m267_e001_lane_e_readiness.py"),
    (
        sys.executable,
        "scripts/check_m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (the lane-E closeout replays the published M267 source, semantic, lowering, runtime-helper, live-runtime, cross-module, and gate proofs, then freezes the summary-chain closeout without introducing new runtime semantics)",
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
    print("[ok] M267-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
