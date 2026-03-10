#!/usr/bin/env python3
"""Run the M256-D004 lane-D readiness chain without recursive npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M256-A001..A003 + M256-B001..B004 + M256-C001..C003 + M256-D001..D004"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (sys.executable, "scripts/check_m256_a001_executable_class_protocol_category_source_closure_contract.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_a001_executable_class_protocol_category_source_closure_contract.py", "-q"),
    (sys.executable, "scripts/check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py", "-q"),
    (sys.executable, "scripts/check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py", "-q"),
    (sys.executable, "scripts/check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py", "-q"),
    (sys.executable, "scripts/check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py", "-q"),
    (sys.executable, "scripts/check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py", "-q"),
    (sys.executable, "scripts/check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py", "-q"),
    (sys.executable, "scripts/check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py", "-q"),
    (sys.executable, "scripts/check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py", "-q"),
    (sys.executable, "scripts/check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py", "-q"),
    (sys.executable, "scripts/check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py", "-q"),
    (sys.executable, "scripts/check_m256_d002_metaclass_graph_and_root_class_baseline_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_d002_metaclass_graph_and_root_class_baseline_core_feature_implementation.py", "-q"),
    (sys.executable, "scripts/check_m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation.py", "-q"),
    (sys.executable, "scripts/check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py", "-q"),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (source closure, object-model sema, executable lowering, realized-graph runtime, and canonical object-sample support must stay aligned without recursive issue runners)"
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
    print("[ok] M256-D004 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
