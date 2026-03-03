#!/usr/bin/env python3
"""Run M248-E012 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m248_e011_ci_governance_gate_and_closeout_policy_performance_and_quality_guardrails_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_e011_ci_governance_gate_and_closeout_policy_performance_and_quality_guardrails_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m248_b012_semantic_lowering_test_architecture_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_b012_semantic_lowering_test_architecture_cross_lane_integration_sync_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m248_e012_ci_governance_gate_and_closeout_policy_cross_lane_integration_sync_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
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
    print("[ok] M248-E012 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
