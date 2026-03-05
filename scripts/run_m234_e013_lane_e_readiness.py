#!/usr/bin/env python3
"""Run M234-E013 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m234_e001_property_conformance_gate_and_docs_contract_and_architecture_freeze_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e001_property_conformance_gate_and_docs_contract_and_architecture_freeze_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e002_property_conformance_gate_and_docs_modular_split_scaffolding_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e002_property_conformance_gate_and_docs_modular_split_scaffolding_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e003_property_conformance_gate_and_docs_core_feature_implementation_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e003_property_conformance_gate_and_docs_core_feature_implementation_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e004_property_conformance_gate_and_docs_core_feature_expansion_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e004_property_conformance_gate_and_docs_core_feature_expansion_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e005_property_conformance_gate_and_docs_edge_case_and_compatibility_completion_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e005_property_conformance_gate_and_docs_edge_case_and_compatibility_completion_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e006_property_conformance_gate_and_docs_edge_case_expansion_and_robustness_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e006_property_conformance_gate_and_docs_edge_case_expansion_and_robustness_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e007_property_conformance_gate_and_docs_diagnostics_hardening_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e007_property_conformance_gate_and_docs_diagnostics_hardening_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e008_property_conformance_gate_and_docs_recovery_and_determinism_hardening_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e008_property_conformance_gate_and_docs_recovery_and_determinism_hardening_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e009_property_conformance_gate_and_docs_conformance_matrix_implementation_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e009_property_conformance_gate_and_docs_conformance_matrix_implementation_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e010_property_conformance_gate_and_docs_conformance_corpus_expansion_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e010_property_conformance_gate_and_docs_conformance_corpus_expansion_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e011_property_conformance_gate_and_docs_performance_and_quality_guardrails_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e011_property_conformance_gate_and_docs_performance_and_quality_guardrails_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e012_property_conformance_gate_and_docs_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e012_property_conformance_gate_and_docs_cross_lane_integration_sync_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m234_e013_property_conformance_gate_and_docs_docs_and_operator_runbook_synchronization_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_e013_property_conformance_gate_and_docs_docs_and_operator_runbook_synchronization_contract.py",
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
    print("[ok] M234-E013 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
