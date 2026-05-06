#!/usr/bin/env python3
"""Unified public workflow runner for the live objc3c command surface."""

from __future__ import annotations

import hashlib
import json
import os
import re
import subprocess
import sys
from dataclasses import asdict
from datetime import datetime, timezone
from pathlib import Path
from time import perf_counter
from typing import Sequence

SCRIPT_ROOT = Path(__file__).resolve().parents[1]
ROOT = Path(__file__).resolve().parents[2]
for import_root in (ROOT, SCRIPT_ROOT):
    if str(import_root) not in sys.path:
        sys.path.insert(0, str(import_root))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.subprocesses import run_capture
from objc3c_tooling.public_workflow_output import extract_public_workflow_report_paths as extract_report_paths

from scripts.objc3c_workflow.action_spec import ActionHandler, ActionSpec
from scripts.objc3c_workflow.commands import extract_output_line, pwsh_file, run, workflow_command
from scripts.objc3c_workflow.environment import (
    MARKDOWN_GLOBS,
    NPX,
    PWSH,
    WORKFLOW_COMMAND_TEXT,
    WORKFLOW_MODULE,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from scripts.objc3c_workflow.npm_surface import describe_package_script_payload
from scripts.objc3c_workflow.registry import ACTION_SPECS
from scripts.objc3c_workflow.reports import emit_json

BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
COMPILE_WRAPPER_SELF_AUDIT_PY = (
    ROOT / "scripts" / "check_objc3c_compile_wrapper_self_audit.py"
)
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
MATRIX_PS1 = ROOT / "scripts" / "run_objc3c_native_fixture_matrix.ps1"
NEGATIVE_EXPECTATIONS_PS1 = ROOT / "scripts" / "check_objc3c_negative_fixture_expectations.ps1"
COMPILER_THROUGHPUT_PS1 = ROOT / "scripts" / "check_objc3c_native_perf_budget.ps1"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROOF_PS1 = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"
SITE_PY = ROOT / "scripts" / "build_site_index.py"
NATIVE_DOCS_PY = ROOT / "scripts" / "build_objc3c_native_docs.py"
PUBLIC_COMMAND_SURFACE_PY = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
PUBLIC_COMMAND_CONTRACT_PY = ROOT / "scripts" / "build_objc3c_public_command_contract.py"
PUBLIC_COMMAND_BUDGET_PY = ROOT / "scripts" / "check_objc3c_public_command_budget.py"
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
REPO_SUPERCLEAN_SURFACE_PY = ROOT / "scripts" / "check_repo_superclean_surface.py"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
RUNNABLE_SHOWCASE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_showcase_end_to_end.py"
GETTING_STARTED_INTEGRATION_PY = ROOT / "scripts" / "check_getting_started_integration.py"
DEVELOPER_TOOLING_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_developer_tooling_integration.py"
RUNNABLE_DEVELOPER_TOOLING_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_developer_tooling_end_to_end.py"
EDITOR_TOOLING_SURFACE_PY = ROOT / "scripts" / "build_objc3c_editor_tooling_surface.py"
FORMAT_OBJC3C_SOURCE_PY = ROOT / "scripts" / "format_objc3c_source.py"
BONUS_EXPERIENCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_bonus_experience_integration.py"
RUNTIME_INSPECTOR_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_runtime_inspector.py"
PERFORMANCE_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_performance.py"
RUNTIME_PERFORMANCE_BENCHMARK_PY = ROOT / "scripts" / "benchmark_objc3c_runtime_performance.py"
COMPILER_THROUGHPUT_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_compiler_throughput_integration.py"
RUNNABLE_COMPILER_THROUGHPUT_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_compiler_throughput_end_to_end.py"
COMPARATIVE_BASELINES_PY = ROOT / "scripts" / "run_objc3c_comparative_baselines.py"
RUNNABLE_PERFORMANCE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_performance_end_to_end.py"
PERFORMANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_performance_integration.py"
RUNTIME_PERFORMANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_runtime_performance_integration.py"
RUNNABLE_RUNTIME_PERFORMANCE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_runtime_performance_end_to_end.py"
CONFORMANCE_CORPUS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_conformance_corpus_integration.py"
RUNNABLE_CONFORMANCE_CORPUS_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_conformance_corpus_end_to_end.py"
STRESS_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_stress_source_surface.py"
FUZZ_SAFETY_PY = ROOT / "scripts" / "run_objc3c_fuzz_safety.py"
LOWERING_RUNTIME_STRESS_PY = ROOT / "scripts" / "run_objc3c_lowering_runtime_stress.py"
MIXED_MODULE_DIFFERENTIAL_PY = ROOT / "scripts" / "run_objc3c_mixed_module_differential.py"
STRESS_MINIMIZATION_PY = ROOT / "scripts" / "run_objc3c_stress_minimization.py"
STRESS_CRASH_TRIAGE_PY = ROOT / "scripts" / "run_objc3c_stress_crash_triage.py"
STRESS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stress_integration.py"
STRESS_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_stress_end_to_end.py"
EXTERNAL_VALIDATION_SURFACE_PY = ROOT / "scripts" / "check_external_validation_source_surface.py"
EXTERNAL_VALIDATION_REPLAY_PY = ROOT / "scripts" / "run_objc3c_external_validation_replay.py"
EXTERNAL_VALIDATION_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_external_repro_corpus.py"
EXTERNAL_VALIDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_external_validation_integration.py"
PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_public_conformance_reporting_source_surface.py"
PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY = ROOT / "scripts" / "check_public_conformance_schema_surface.py"
PUBLIC_CONFORMANCE_SCORECARD_PY = ROOT / "scripts" / "build_objc3c_public_conformance_scorecard.py"
PUBLIC_CONFORMANCE_REPORT_PY = ROOT / "scripts" / "publish_objc3c_public_conformance_report.py"
PUBLIC_CONFORMANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_public_conformance_reporting_integration.py"
PUBLIC_CONFORMANCE_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_public_conformance_reporting_end_to_end.py"
PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_performance_governance_source_surface.py"
PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY = ROOT / "scripts" / "check_performance_governance_schema_surface.py"
PERFORMANCE_GOVERNANCE_DASHBOARD_PY = ROOT / "scripts" / "build_objc3c_performance_dashboard.py"
PERFORMANCE_GOVERNANCE_REPORT_PY = ROOT / "scripts" / "publish_objc3c_performance_report.py"
PERFORMANCE_GOVERNANCE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_performance_governance_integration.py"
PERFORMANCE_GOVERNANCE_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_performance_governance_end_to_end.py"
RELEASE_FOUNDATION_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_release_foundation_source_surface.py"
RELEASE_FOUNDATION_SCHEMA_SURFACE_PY = ROOT / "scripts" / "check_release_foundation_schema_surface.py"
RELEASE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_release_manifest.py"
RELEASE_PROVENANCE_PY = ROOT / "scripts" / "publish_objc3c_release_provenance.py"
RELEASE_FOUNDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_release_foundation_integration.py"
PACKAGING_CHANNELS_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_packaging_channels_source_surface.py"
PACKAGING_CHANNELS_SCHEMA_SURFACE_PY = ROOT / "scripts" / "check_packaging_channels_schema_surface.py"
PACKAGE_CHANNELS_BUILD_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
PACKAGING_CHANNELS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_packaging_channels_integration.py"
PACKAGING_CHANNELS_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"
PLATFORM_SUPPORT_MATRIX_PY = ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
PLATFORM_HARDENING_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_platform_hardening_integration.py"
RUNNABLE_PLATFORM_HARDENING_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_platform_hardening_end_to_end.py"
RELEASE_OPERATIONS_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_release_operations_source_surface.py"
RELEASE_OPERATIONS_SCHEMA_SURFACE_PY = ROOT / "scripts" / "check_release_operations_schema_surface.py"
UPDATE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_update_manifest.py"
RELEASE_OPERATIONS_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_release_operations_metadata.py"
RELEASE_OPERATIONS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_release_operations_integration.py"
RELEASE_OPERATIONS_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_release_operations_end_to_end.py"
DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_distribution_credibility_source_surface.py"
DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY = ROOT / "scripts" / "check_distribution_credibility_schema_surface.py"
DISTRIBUTION_CREDIBILITY_DASHBOARD_PY = ROOT / "scripts" / "build_objc3c_distribution_credibility_dashboard.py"
DISTRIBUTION_CREDIBILITY_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_distribution_trust_report.py"
DISTRIBUTION_CREDIBILITY_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_distribution_credibility_integration.py"
DISTRIBUTION_CREDIBILITY_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_distribution_credibility_end_to_end.py"
SECURITY_HARDENING_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_security_hardening_source_surface.py"
SECURITY_HARDENING_SCHEMA_SURFACE_PY = ROOT / "scripts" / "check_security_hardening_schema_surface.py"
SECURITY_HARDENING_RESPONSE_DRILL_PY = ROOT / "scripts" / "check_security_hardening_response_drill.py"
SECURITY_HARDENING_RUNTIME_HARDENING_PY = ROOT / "scripts" / "check_security_hardening_runtime_hardening.py"
SECURITY_HARDENING_POSTURE_PY = ROOT / "scripts" / "build_objc3c_security_posture.py"
SECURITY_HARDENING_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_security_advisories.py"
SECURITY_HARDENING_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_security_hardening_integration.py"
SECURITY_HARDENING_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_security_hardening_end_to_end.py"
STDLIB_SURFACE_PY = ROOT / "scripts" / "check_stdlib_surface.py"
MATERIALIZE_STDLIB_PY = ROOT / "scripts" / "materialize_objc3c_stdlib_workspace.py"
STDLIB_FOUNDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_foundation_integration.py"
STDLIB_ADVANCED_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_advanced_integration.py"
STDLIB_PROGRAM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_program_integration.py"
RUNNABLE_STDLIB_FOUNDATION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_foundation_end_to_end.py"
RUNNABLE_STDLIB_ADVANCED_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_advanced_end_to_end.py"
RUNNABLE_STDLIB_PROGRAM_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_program_end_to_end.py"
PROJECT_TEMPLATE_MATERIALIZER_PY = ROOT / "scripts" / "materialize_objc3c_project_template.py"
CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY = ROOT / "scripts" / "materialize_objc3c_canonical_application_workspace.py"
APPLICATION_ARCHITECTURE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_application_architecture_integration.py"
RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_application_architecture_end_to_end.py"
PACKAGE_LOCK_PY = ROOT / "scripts" / "build_objc3c_package_lock.py"
PACKAGE_AUTHORING_WORKFLOW_PY = ROOT / "scripts" / "check_objc3c_package_authoring_workflow.py"
PACKAGE_MIRROR_REPRODUCIBILITY_PY = ROOT / "scripts" / "check_objc3c_package_registry_mirror_reproducibility.py"
PACKAGE_ECOSYSTEM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_package_ecosystem_integration.py"
RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_package_ecosystem_end_to_end.py"
LONG_HORIZON_OPERATIONS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_long_horizon_operations_integration.py"
LONG_HORIZON_OPERATIONS_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_long_horizon_operations_metadata.py"
ADOPTION_LEGIBILITY_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_adoption_legibility_integration.py"
ADOPTION_LEGIBILITY_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_adoption_legibility_metadata.py"
GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_governance_sustainability_integration.py"
GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY = ROOT / "scripts" / "publish_objc3c_governance_sustainability_metadata.py"
PLANNING_ISSUE_PUBLISHER_PY = ROOT / "scripts" / "publish_objc3c_planning_issues.py"
PLANNING_PUBLICATION_AUDIT_PY = ROOT / "scripts" / "audit_objc3c_planning_publication.py"
LLVM_CAPABILITIES_PROBE_PY = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
DEPENDENCY_BOUNDARIES_PY = ROOT / "scripts" / "check_objc3c_dependency_boundaries.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
SOURCE_HYGIENE_AUTHENTICITY_PY = ROOT / "scripts" / "check_source_hygiene_authenticity.py"
SOURCE_HYGIENE_HARD_CUTOVER_PY = ROOT / "scripts" / "check_source_hygiene_hard_cutover.py"
RUNNABLE_BONUS_EXPERIENCE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_bonus_experience_end_to_end.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"
RUNTIME_ARCHITECTURE_PROOF_PACKET_PY = ROOT / "scripts" / "check_objc3c_runtime_architecture_proof_packet.py"
RUNTIME_ARCHITECTURE_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
RUNNABLE_BOOTSTRAP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_bootstrap_end_to_end.py"
RUNNABLE_BLOCK_ARC_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_block_arc_conformance.py"
RUNNABLE_BLOCK_ARC_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_block_arc_end_to_end.py"
RUNNABLE_CONCURRENCY_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_concurrency_conformance.py"
RUNNABLE_CONCURRENCY_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_concurrency_end_to_end.py"
RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_object_model_conformance.py"
RUNNABLE_OBJECT_MODEL_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_object_model_end_to_end.py"
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_conformance.py"
RUNNABLE_STORAGE_REFLECTION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_end_to_end.py"
RUNNABLE_ERROR_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_error_conformance.py"
RUNNABLE_ERROR_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_error_end_to_end.py"
RUNNABLE_INTEROP_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_interop_conformance.py"
RUNNABLE_INTEROP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_interop_end_to_end.py"
RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_conformance.py"
RUNNABLE_METAPROGRAMMING_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_end_to_end.py"
RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY = ROOT / "scripts" / "check_objc3c_runnable_release_candidate_conformance.py"
RUNNABLE_RELEASE_CANDIDATE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_release_candidate_end_to_end.py"
FRONTEND_C_API_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_DEVELOPER_TOOLING_SOURCE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_PLAYGROUND_SOURCE = DEFAULT_DEVELOPER_TOOLING_SOURCE
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
PLAYGROUND_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "playground"
PLAYGROUND_REPORT_ROOT = ROOT / "tmp" / "reports" / "playground"
PLAYGROUND_WORKSPACE_CONTRACT_ID = "objc3c.playground.workspace.v1"
REPO_SUPERCLEAN_SOURCE_OF_TRUTH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "repo_superclean_source_of_truth.json"
SHOWCASE_PORTFOLIO_JSON = ROOT / "showcase" / "portfolio.json"
SHOWCASE_TUTORIAL_WALKTHROUGH_JSON = ROOT / "showcase" / "tutorial_walkthrough.json"
def run_steps(actions: Sequence[str]) -> int:
    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


def action_build_default(_: list[str]) -> int:
    return run_steps(["build-native-binaries"])


def action_build_native_binaries(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only")


def action_build_native_contracts(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "contracts-binary")


def action_build_native_full(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "full")


def action_build_native_reconfigure(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only", "-ForceReconfigure")


def action_build_site(_: list[str]) -> int:
    rc = run([sys.executable, str(SITE_PY)])
    if rc != 0:
        return rc
    return run([NPX, "prettier", "--write", "site/index.md"])


def action_check_site(_: list[str]) -> int:
    return run([sys.executable, str(SITE_PY), "--check"])


def action_build_native_docs(_: list[str]) -> int:
    return run([sys.executable, str(NATIVE_DOCS_PY)])


def action_check_native_docs(_: list[str]) -> int:
    return run([sys.executable, str(NATIVE_DOCS_PY), "--check"])


def action_build_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY)])


def action_check_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"])


def action_build_public_command_contract(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_CONTRACT_PY)])


def action_check_public_command_contract(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_CONTRACT_PY), "--check"])


def action_check_public_command_budget(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_BUDGET_PY)])


def action_check_documentation_surface(_: list[str]) -> int:
    return run([sys.executable, str(DOCUMENTATION_SURFACE_PY)])


def action_check_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--check", *MARKDOWN_GLOBS])


def action_format_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--write", *MARKDOWN_GLOBS])


def action_lint_markdown(_: list[str]) -> int:
    return run([NPX, "markdownlint-cli2", *MARKDOWN_GLOBS])


def action_check_dependency_boundaries(_: list[str]) -> int:
    return run([sys.executable, str(DEPENDENCY_BOUNDARIES_PY), "--strict"])


def action_check_llvm_capabilities(_: list[str]) -> int:
    return run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            "tmp/artifacts/objc3c-native/llvm_capabilities/summary.json",
        ]
    )


def action_check_release_evidence(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_EVIDENCE_PY)])


def action_check_source_hygiene_authenticity(_: list[str]) -> int:
    return run([sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)])


def action_check_source_hygiene_hard_cutover(_: list[str]) -> int:
    return run([sys.executable, str(SOURCE_HYGIENE_HARD_CUTOVER_PY)])


def action_check_task_hygiene(_: list[str]) -> int:
    return run([sys.executable, str(TASK_HYGIENE_PY)])


def action_check_repo_superclean_surface(_: list[str]) -> int:
    return run([sys.executable, str(REPO_SUPERCLEAN_SURFACE_PY)])


def action_check_showcase_surface(rest: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_SURFACE_PY), *rest])


def action_check_stdlib_surface(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_SURFACE_PY)])


def action_materialize_stdlib_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(MATERIALIZE_STDLIB_PY), *rest])


def action_materialize_canonical_application_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY), *rest])


def action_validate_showcase_runtime(rest: list[str]) -> int:
    return pwsh_file(SHOWCASE_RUNTIME_PS1, *rest)


def action_validate_showcase(_: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_INTEGRATION_PY)])


def action_validate_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)])


def action_validate_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)])


def action_validate_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)])


def action_validate_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(APPLICATION_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_runnable_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY)])


def action_build_package_lock(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_LOCK_PY)])


def action_validate_package_authoring(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_AUTHORING_WORKFLOW_PY)])


def action_validate_package_mirror(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_MIRROR_REPRODUCIBILITY_PY)])


def action_validate_package_ecosystem(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_ECOSYSTEM_INTEGRATION_PY)])


def action_validate_runnable_package_ecosystem(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY)])


def action_validate_long_horizon_operations(_: list[str]) -> int:
    return run([sys.executable, str(LONG_HORIZON_OPERATIONS_INTEGRATION_PY)])


def action_publish_long_horizon_operations(_: list[str]) -> int:
    return run([sys.executable, str(LONG_HORIZON_OPERATIONS_PUBLICATION_PY)])


def action_validate_adoption_legibility(_: list[str]) -> int:
    return run([sys.executable, str(ADOPTION_LEGIBILITY_INTEGRATION_PY)])


def action_publish_adoption_legibility(_: list[str]) -> int:
    return run([sys.executable, str(ADOPTION_LEGIBILITY_PUBLICATION_PY)])


def action_validate_governance_sustainability(_: list[str]) -> int:
    return run([sys.executable, str(GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY)])


def action_publish_governance_sustainability(_: list[str]) -> int:
    return run([sys.executable, str(GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY)])


def action_publish_planning_issues(rest: list[str]) -> int:
    return run([sys.executable, str(PLANNING_ISSUE_PUBLISHER_PY), *rest])


def action_check_planning_publication_drift(rest: list[str]) -> int:
    return run([sys.executable, str(PLANNING_PUBLICATION_AUDIT_PY), "--check", *rest])


def action_validate_runnable_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_ADVANCED_E2E_PY)])


def action_validate_runnable_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_PROGRAM_E2E_PY)])


def action_validate_runnable_showcase(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_SHOWCASE_E2E_PY)])


def action_validate_getting_started(_: list[str]) -> int:
    return run([sys.executable, str(GETTING_STARTED_INTEGRATION_PY)])


def action_validate_documentation_surface(_: list[str]) -> int:
    commands = [
        [sys.executable, str(SITE_PY)],
        [sys.executable, str(NATIVE_DOCS_PY)],
        [sys.executable, str(PUBLIC_COMMAND_SURFACE_PY)],
        [sys.executable, str(SITE_PY), "--check"],
        [sys.executable, str(NATIVE_DOCS_PY), "--check"],
        [sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"],
        [sys.executable, str(DOCUMENTATION_SURFACE_PY)],
    ]
    for command in commands:
        rc = run(command)
        if rc != 0:
            return rc
    return 0


def action_validate_repo_superclean(_: list[str]) -> int:
    return run_composite_validation(
        "validate-repo-superclean",
        [
            ("build-native-contracts", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(BUILD_PS1), "-ExecutionMode", "contracts-binary"]),
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("source-hygiene", [sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)]),
        ],
    )


def action_compile_objc3c(rest: list[str]) -> int:
    return pwsh_file(COMPILE_PS1, *rest)


def _parse_developer_tooling_invocation(rest: list[str]) -> tuple[str, list[str]]:
    if rest and not rest[0].startswith("--"):
        source_text = rest[0]
        passthrough = rest[1:]
    else:
        source_text = str(DEFAULT_DEVELOPER_TOOLING_SOURCE.relative_to(ROOT).as_posix())
        passthrough = rest
    for forbidden in (
        "--summary-out",
        "--dump-summary-json",
        "--dump-observability-json",
        "--dump-playground-repro-json",
        "--dump-runtime-inspector-json",
        "--dump-stage-trace-json",
    ):
        if forbidden in passthrough:
            raise ValueError(f"{forbidden} is managed by the public developer-tooling action")
    return source_text, passthrough


def _parse_playground_invocation(rest: list[str]) -> tuple[str, list[str]]:
    if rest and not rest[0].startswith("--"):
        source_text = rest[0]
        passthrough = rest[1:]
    else:
        source_text = str(DEFAULT_PLAYGROUND_SOURCE.relative_to(ROOT).as_posix())
        passthrough = rest
    for forbidden in (
        "--out-dir",
        "--emit-prefix",
        "--summary-out",
        "--dump-summary-json",
        "--dump-observability-json",
        "--dump-playground-repro-json",
        "--dump-runtime-inspector-json",
        "--dump-stage-trace-json",
    ):
        if forbidden in passthrough:
            raise ValueError(f"{forbidden} is managed by the public playground action")
    return source_text, passthrough


def _resolve_source_path(source_text: str) -> tuple[Path, str]:
    candidate = Path(source_text)
    resolved = candidate if candidate.is_absolute() else ROOT / candidate
    resolved = resolved.resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"playground source not found: {source_text}")
    try:
        display = resolved.relative_to(ROOT).as_posix()
    except ValueError:
        display = resolved.as_posix()
    return resolved, display


def _slugify_playground_workspace(source_display: str) -> str:
    safe_stem = re.sub(r"[^a-z0-9]+", "-", Path(source_display).stem.lower()).strip("-")
    if not safe_stem:
        safe_stem = "source"
    digest = hashlib.sha256(source_display.encode("utf-8")).hexdigest()[:12]
    return f"{safe_stem}-{digest}"


def _ensure_frontend_runner_ready() -> int:
    if not (ROOT / "native" / "objc3c" / "src" / "main.cpp").is_file() and FRONTEND_C_API_RUNNER_EXE.is_file():
        return 0
    return execute_registered_action("build-native-binaries", [])


def _run_playground_workspace(
    rest: list[str],
    *,
    emit_payload: bool,
) -> int:
    try:
        source_text, passthrough = _parse_playground_invocation(rest)
        _, source_display = _resolve_source_path(source_text)
    except (ValueError, FileNotFoundError) as exc:
        print(str(exc), file=sys.stderr)
        return 2

    rc = _ensure_frontend_runner_ready()
    if rc != 0:
        return rc

    workspace_id = _slugify_playground_workspace(source_display)
    workspace_root = PLAYGROUND_ARTIFACT_ROOT / workspace_id
    artifact_root = workspace_root / "build"
    report_root = PLAYGROUND_REPORT_ROOT / workspace_id
    workspace_manifest_path = workspace_root / "workspace.json"
    summary_path = report_root / "compile-summary.json"
    dump_path = report_root / "playground-repro.json"

    artifact_root.mkdir(parents=True, exist_ok=True)
    report_root.mkdir(parents=True, exist_ok=True)

    artifact_root_rel = artifact_root.relative_to(ROOT).as_posix()
    summary_path_rel = summary_path.relative_to(ROOT).as_posix()

    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(
        [
            str(FRONTEND_C_API_RUNNER_EXE),
            source_display,
            "--out-dir",
            artifact_root_rel,
            "--emit-prefix",
            "module",
            "--summary-out",
            summary_path_rel,
            "--dump-playground-repro-json",
            *passthrough,
        ],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        if result.stdout:
            sys.stdout.write(result.stdout)
        return result.returncode

    try:
        playground_payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        print(f"playground-workspace: invalid JSON from frontend runner: {exc}", file=sys.stderr)
        return 1

    dump_path.write_text(json.dumps(playground_payload, indent=2) + "\n", encoding="utf-8")

    editor_result = subprocess.run(
        [sys.executable, str(EDITOR_TOOLING_SURFACE_PY), source_display],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if editor_result.stdout:
        sys.stdout.write(editor_result.stdout)
    if editor_result.stderr:
        sys.stderr.write(editor_result.stderr)
    if editor_result.returncode != 0:
        return editor_result.returncode

    editor_surface_path_text = extract_output_line(editor_result.stdout, "dump_path:")
    capabilities_path_text = extract_output_line(editor_result.stdout, "capabilities_path:")
    navigation_path_text = extract_output_line(editor_result.stdout, "navigation_path:")
    formatter_path_text = extract_output_line(editor_result.stdout, "formatter_path:")
    debug_path_text = extract_output_line(editor_result.stdout, "debug_path:")
    if not editor_surface_path_text:
        print("playground-workspace: editor tooling action did not publish dump_path", file=sys.stderr)
        return 1
    editor_surface_path = ROOT / editor_surface_path_text
    if not editor_surface_path.is_file():
        print(f"playground-workspace: missing editor tooling surface {editor_surface_path_text}", file=sys.stderr)
        return 1
    editor_surface_payload = load_json(editor_surface_path)
    formatter_payload = editor_surface_payload.get("formatter", {})
    debug_payload = editor_surface_payload.get("debug", {})
    workspace_drill_commands = {
        "inspect_editor_tooling": f"{WORKFLOW_COMMAND_TEXT} inspect-editor-tooling {source_display}",
        "format_preview": f"{WORKFLOW_COMMAND_TEXT} format-objc3c {source_display}",
        "object_symbol_inventory": str(debug_payload.get("object_symbol_inventory_command", "")),
    }

    workspace_payload = {
        "contract_id": PLAYGROUND_WORKSPACE_CONTRACT_ID,
        "schema_version": 1,
        "workspace_id": workspace_id,
        "source_path": source_display,
        "workspace_root": workspace_root.relative_to(ROOT).as_posix(),
        "artifact_root": artifact_root.relative_to(ROOT).as_posix(),
        "report_root": report_root.relative_to(ROOT).as_posix(),
        "emit_prefix": "module",
        "summary_path": summary_path.relative_to(ROOT).as_posix(),
        "playground_payload_path": dump_path.relative_to(ROOT).as_posix(),
        "playground_payload_contract_id": playground_payload.get("contract_id"),
        "public_actions": [
            "materialize-playground-workspace",
            "compile-objc3c",
            "inspect-playground-repro",
            "inspect-compile-observability",
            "inspect-editor-tooling",
            "format-objc3c",
            "trace-compile-stages",
            "validate-developer-tooling",
        ],
        "compile_profile": playground_payload.get("compile_profile", {}),
        "artifact_paths": playground_payload.get("artifact_paths", {}),
        "showcase_examples": playground_payload.get("showcase_examples", []),
        "repro_command": playground_payload.get("dump_commands", {}).get("repro_runner", ""),
        "editor_tooling": {
            "editor_surface_path": editor_surface_path_text,
            "language_server_capabilities_path": capabilities_path_text,
            "navigation_path": navigation_path_text,
            "formatter_path": formatter_path_text,
            "debug_path": debug_path_text,
            "formatted_output_path": formatter_payload.get("formatted_output_path"),
            "format_preview_supported": formatter_payload.get("supported"),
            "debugger_model": debug_payload.get("debugger_model", ""),
            "declaration_breakpoint_anchor_count": debug_payload.get("declaration_breakpoint_anchor_count", 0),
            "statement_level_stepping": debug_payload.get("statement_level_stepping"),
        },
        "workspace_drill_commands": workspace_drill_commands,
    }
    workspace_manifest_path.write_text(
        json.dumps(workspace_payload, indent=2) + "\n",
        encoding="utf-8",
    )

    if emit_payload:
        sys.stdout.write(json.dumps(playground_payload, indent=2) + "\n")
    print(f"workspace_path: {workspace_manifest_path.relative_to(ROOT).as_posix()}")
    print(f"summary_path: {summary_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"artifact_root: {artifact_root.relative_to(ROOT).as_posix()}")
    return 0


def _write_json_capture(path: Path, stdout: str) -> int:
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        print(f"developer-tooling-dump: invalid JSON from frontend runner: {exc}", file=sys.stderr)
        return 1
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return 0


def _run_developer_tooling_dump(action_name: str,
                                dump_flag: str,
                                dump_filename: str,
                                rest: list[str]) -> int:
    try:
        source_text, passthrough = _parse_developer_tooling_invocation(rest)
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 2
    rc = _ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    summary_path = PUBLIC_WORKFLOW_REPORT_ROOT / f"{action_name}-summary.json"
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / dump_filename
    command = [
        str(FRONTEND_C_API_RUNNER_EXE),
        source_text,
        "--summary-out",
        str(summary_path),
        dump_flag,
        *passthrough,
    ]
    result = run_capture(command)
    if result.returncode != 0:
        return result.returncode
    rc = _write_json_capture(dump_path, result.stdout)
    if rc != 0:
        return rc
    print(f"summary_path: {summary_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0


def action_inspect_compile_observability(rest: list[str]) -> int:
    return _run_developer_tooling_dump(
        "inspect-compile-observability",
        "--dump-observability-json",
        "compile-observability.json",
        rest,
    )


def action_inspect_runtime_inspector(rest: list[str]) -> int:
    return _run_developer_tooling_dump(
        "inspect-runtime-inspector",
        "--dump-runtime-inspector-json",
        "runtime-inspector.json",
        rest,
    )


def action_inspect_editor_tooling(rest: list[str]) -> int:
    rc = _ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return run([sys.executable, str(EDITOR_TOOLING_SURFACE_PY), *rest])


def action_format_objc3c(rest: list[str]) -> int:
    return run([sys.executable, str(FORMAT_OBJC3C_SOURCE_PY), *rest])


def action_inspect_capability_explorer(rest: list[str]) -> int:
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "capability-explorer.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    rc = run(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            str(dump_path),
            *rest,
        ]
    )
    if rc == 0:
        print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
        print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return rc


def action_inspect_playground_repro(rest: list[str]) -> int:
    return _run_playground_workspace(rest, emit_payload=True)


def action_materialize_playground_workspace(rest: list[str]) -> int:
    return _run_playground_workspace(rest, emit_payload=False)


def action_trace_compile_stages(rest: list[str]) -> int:
    return _run_developer_tooling_dump(
        "trace-compile-stages",
        "--dump-stage-trace-json",
        "compile-stage-trace.json",
        rest,
    )


def action_validate_developer_tooling(_: list[str]) -> int:
    return run([sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)])


def action_validate_runnable_developer_tooling(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_DEVELOPER_TOOLING_E2E_PY)])


def action_validate_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)])


def action_benchmark_runtime_inspector(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_INSPECTOR_BENCHMARK_PY), *rest])


def action_benchmark_performance(rest: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_BENCHMARK_PY), *rest])


def action_benchmark_runtime_performance(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_PERFORMANCE_BENCHMARK_PY), *rest])


def action_validate_runtime_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_PERFORMANCE_INTEGRATION_PY)])


def action_validate_runnable_runtime_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RUNTIME_PERFORMANCE_E2E_PY)])


def action_benchmark_comparative_baselines(rest: list[str]) -> int:
    return run([sys.executable, str(COMPARATIVE_BASELINES_PY), *rest])


def action_validate_runnable_performance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PERFORMANCE_E2E_PY)])


def action_validate_performance_foundation(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_INTEGRATION_PY)])


def action_validate_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)])


def action_validate_runnable_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONFORMANCE_CORPUS_E2E_PY)])


def action_check_stress_surface(_: list[str]) -> int:
    return run([sys.executable, str(STRESS_SOURCE_SURFACE_PY)])


def action_test_fuzz_safety(rest: list[str]) -> int:
    return run([sys.executable, str(FUZZ_SAFETY_PY), *rest])


def action_test_lowering_runtime_stress(rest: list[str]) -> int:
    return run([sys.executable, str(LOWERING_RUNTIME_STRESS_PY), *rest])


def action_test_mixed_module_differential(rest: list[str]) -> int:
    return run([sys.executable, str(MIXED_MODULE_DIFFERENTIAL_PY), *rest])


def action_test_stress_minimization(rest: list[str]) -> int:
    return run([sys.executable, str(STRESS_MINIMIZATION_PY), *rest])


def action_test_stress_crash_triage(rest: list[str]) -> int:
    return run([sys.executable, str(STRESS_CRASH_TRIAGE_PY), *rest])


def action_validate_stress(_: list[str]) -> int:
    return run_composite_validation(
        "validate-stress",
        [
            ("check-stress-surface", [sys.executable, str(STRESS_SOURCE_SURFACE_PY)]),
            ("test-fuzz-safety", [sys.executable, str(FUZZ_SAFETY_PY)]),
            ("test-lowering-runtime-stress", [sys.executable, str(LOWERING_RUNTIME_STRESS_PY)]),
            ("test-mixed-module-differential", [sys.executable, str(MIXED_MODULE_DIFFERENTIAL_PY)]),
            ("test-stress-minimization", [sys.executable, str(STRESS_MINIMIZATION_PY)]),
            ("test-stress-crash-triage", [sys.executable, str(STRESS_CRASH_TRIAGE_PY)]),
        ],
    )


def action_validate_stress_integration(_: list[str]) -> int:
    return run([sys.executable, str(STRESS_INTEGRATION_PY)])


def action_validate_stress_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(STRESS_END_TO_END_PY)])


def action_check_external_validation_surface(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_SURFACE_PY)])


def action_test_external_validation_replay(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_REPLAY_PY)])


def action_publish_external_repro_corpus(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_PUBLICATION_PY)])


def action_validate_external_validation(_: list[str]) -> int:
    return run_composite_validation(
        "validate-external-validation",
        [
            ("check-external-validation-surface", [sys.executable, str(EXTERNAL_VALIDATION_SURFACE_PY)]),
            ("test-external-validation-replay", [sys.executable, str(EXTERNAL_VALIDATION_REPLAY_PY)]),
            ("publish-external-repro-corpus", [sys.executable, str(EXTERNAL_VALIDATION_PUBLICATION_PY)]),
        ],
    )


def action_validate_external_validation_integration(_: list[str]) -> int:
    return run([sys.executable, str(EXTERNAL_VALIDATION_INTEGRATION_PY)])


def action_check_public_conformance_reporting_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY)])


def action_check_public_conformance_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY)])


def action_build_public_conformance_scorecard(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_SCORECARD_PY)])


def action_publish_public_conformance_report(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_REPORT_PY)])


def action_validate_public_conformance_reporting(_: list[str]) -> int:
    return run_composite_validation(
        "validate-public-conformance-reporting",
        [
            ("check-public-conformance-reporting-surface", [sys.executable, str(PUBLIC_CONFORMANCE_SOURCE_SURFACE_PY)]),
            ("check-public-conformance-schema-surface", [sys.executable, str(PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PY)]),
            ("build-public-conformance-scorecard", [sys.executable, str(PUBLIC_CONFORMANCE_SCORECARD_PY)]),
            ("publish-public-conformance-report", [sys.executable, str(PUBLIC_CONFORMANCE_REPORT_PY)]),
        ],
    )


def action_validate_public_conformance_reporting_integration(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_INTEGRATION_PY)])


def action_validate_public_conformance_reporting_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_CONFORMANCE_END_TO_END_PY)])


def action_check_performance_governance_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY)])


def action_check_performance_governance_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY)])


def action_build_performance_dashboard(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_DASHBOARD_PY)])


def action_publish_performance_report(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_REPORT_PY)])


def action_validate_performance_governance(_: list[str]) -> int:
    return run_composite_validation(
        "validate-performance-governance",
        [
            ("validate-performance-foundation", [sys.executable, str(PERFORMANCE_INTEGRATION_PY)]),
            ("validate-compiler-throughput", [sys.executable, str(COMPILER_THROUGHPUT_INTEGRATION_PY)]),
            ("validate-runtime-performance", [sys.executable, str(RUNTIME_PERFORMANCE_INTEGRATION_PY)]),
            ("check-performance-governance-surface", [sys.executable, str(PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY)]),
            ("check-performance-governance-schema-surface", [sys.executable, str(PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY)]),
            ("build-performance-dashboard", [sys.executable, str(PERFORMANCE_GOVERNANCE_DASHBOARD_PY)]),
            ("publish-performance-report", [sys.executable, str(PERFORMANCE_GOVERNANCE_REPORT_PY)]),
        ],
    )


def action_validate_performance_governance_integration(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_INTEGRATION_PY)])


def action_validate_performance_governance_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_END_TO_END_PY)])


def action_check_release_foundation_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)])


def action_check_release_foundation_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_FOUNDATION_SCHEMA_SURFACE_PY)])


def action_build_release_manifest(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_MANIFEST_PY)])


def action_publish_release_provenance(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_PROVENANCE_PY)])


def action_validate_release_foundation(_: list[str]) -> int:
    return run_composite_validation(
        "validate-release-foundation",
        [
            ("validate-performance-governance", workflow_command("validate-performance-governance")),
            ("validate-runnable-release-candidate", workflow_command("validate-runnable-release-candidate")),
            ("check-release-evidence", [sys.executable, str(ROOT / "scripts" / "check_release_evidence.py")]),
            ("check-release-foundation-surface", [sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)]),
            ("check-release-foundation-schema-surface", [sys.executable, str(RELEASE_FOUNDATION_SCHEMA_SURFACE_PY)]),
            ("build-release-manifest", [sys.executable, str(RELEASE_MANIFEST_PY)]),
            ("publish-release-provenance", [sys.executable, str(RELEASE_PROVENANCE_PY)]),
        ],
    )


def action_check_packaging_channels_surface(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)])


def action_check_packaging_channels_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_SCHEMA_SURFACE_PY)])


def action_build_package_channels(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)])


def action_build_platform_support_matrix(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_SUPPORT_MATRIX_PY)])


def action_validate_packaging_channels(_: list[str]) -> int:
    return run_composite_validation(
        "validate-packaging-channels",
        [
            ("validate-release-foundation", workflow_command("validate-release-foundation")),
            ("check-packaging-channels-surface", [sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)]),
            ("check-packaging-channels-schema-surface", [sys.executable, str(PACKAGING_CHANNELS_SCHEMA_SURFACE_PY)]),
            ("build-package-channels", [sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)]),
        ],
    )


def action_validate_packaging_channels_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_END_TO_END_PY)])


def action_validate_platform_hardening(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_HARDENING_INTEGRATION_PY)])


def action_validate_platform_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PLATFORM_HARDENING_E2E_PY)])


def action_check_release_operations_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_SOURCE_SURFACE_PY)])


def action_check_release_operations_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_SCHEMA_SURFACE_PY)])


def action_build_update_manifest(_: list[str]) -> int:
    return run([sys.executable, str(UPDATE_MANIFEST_PY)])


def action_publish_release_operations(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_PUBLICATION_PY)])


def action_validate_release_operations(_: list[str]) -> int:
    rc = run_composite_validation(
        "validate-release-operations",
        [
            ("validate-packaging-channels", workflow_command("validate-packaging-channels")),
            ("check-release-operations-surface", [sys.executable, str(RELEASE_OPERATIONS_SOURCE_SURFACE_PY)]),
            ("check-release-operations-schema-surface", [sys.executable, str(RELEASE_OPERATIONS_SCHEMA_SURFACE_PY)]),
            ("build-update-manifest", [sys.executable, str(UPDATE_MANIFEST_PY)]),
            ("publish-release-operations", [sys.executable, str(RELEASE_OPERATIONS_PUBLICATION_PY)]),
        ],
    )
    if rc != 0:
        return rc
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY), "--skip-upstream"])


def action_validate_release_operations_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY)])


def action_check_distribution_credibility_surface(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY)])


def action_check_distribution_credibility_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY)])


def action_build_distribution_credibility_dashboard(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_DASHBOARD_PY)])


def action_publish_distribution_credibility(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_PUBLICATION_PY)])


def action_validate_distribution_credibility(_: list[str]) -> int:
    return run_composite_validation(
        "validate-distribution-credibility",
        [
            ("validate-release-operations", workflow_command("validate-release-operations")),
            ("check-distribution-credibility-surface", [sys.executable, str(DISTRIBUTION_CREDIBILITY_SOURCE_SURFACE_PY)]),
            ("check-distribution-credibility-schema-surface", [sys.executable, str(DISTRIBUTION_CREDIBILITY_SCHEMA_SURFACE_PY)]),
            ("build-distribution-credibility-dashboard", [sys.executable, str(DISTRIBUTION_CREDIBILITY_DASHBOARD_PY)]),
            ("publish-distribution-credibility", [sys.executable, str(DISTRIBUTION_CREDIBILITY_PUBLICATION_PY)]),
        ],
    )


def action_validate_distribution_credibility_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(DISTRIBUTION_CREDIBILITY_END_TO_END_PY)])


def action_check_security_hardening_surface(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)])


def action_check_security_hardening_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_SCHEMA_SURFACE_PY)])


def action_build_security_posture(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_POSTURE_PY)])


def action_publish_security_advisories(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)])


def action_validate_security_hardening(_: list[str]) -> int:
    return run_composite_validation(
        "validate-security-hardening",
        [
            ("check-security-response-drill", [sys.executable, str(SECURITY_HARDENING_RESPONSE_DRILL_PY)]),
            ("check-security-runtime-hardening", [sys.executable, str(SECURITY_HARDENING_RUNTIME_HARDENING_PY)]),
            ("check-security-hardening-surface", [sys.executable, str(SECURITY_HARDENING_SOURCE_SURFACE_PY)]),
            ("check-security-hardening-schema-surface", [sys.executable, str(SECURITY_HARDENING_SCHEMA_SURFACE_PY)]),
            ("build-security-posture", [sys.executable, str(SECURITY_HARDENING_POSTURE_PY)]),
            ("publish-security-advisories", [sys.executable, str(SECURITY_HARDENING_PUBLICATION_PY)]),
        ],
    )


def action_validate_security_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(SECURITY_HARDENING_END_TO_END_PY)])


def action_inspect_bonus_tool_integration(_: list[str]) -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if native_main.is_file():
        rc = execute_registered_action("build-native-contracts", [])
        if rc != 0:
            return rc
    if not REPO_SUPERCLEAN_SOURCE_OF_TRUTH.is_file():
        print(
            f"missing source-of-truth artifact: {REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix()}",
            file=sys.stderr,
        )
        return 1
    source_of_truth = json.loads(REPO_SUPERCLEAN_SOURCE_OF_TRUTH.read_text(encoding="utf-8"))
    portfolio = json.loads(SHOWCASE_PORTFOLIO_JSON.read_text(encoding="utf-8"))
    walkthrough = json.loads(SHOWCASE_TUTORIAL_WALKTHROUGH_JSON.read_text(encoding="utf-8"))
    integration_surface = source_of_truth.get("bonus_tool_integration_surface")
    if not isinstance(integration_surface, dict):
        print("repo superclean artifact missing bonus_tool_integration_surface", file=sys.stderr)
        return 1
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "bonus-tool-integration.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "contract_id": "objc3c.bonus.tool.integration.surface.v1",
        "schema_version": 1,
        "source_of_truth_artifact": REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix(),
        "integration_surface": integration_surface,
        "bonus_experience_surfaces": source_of_truth.get("bonus_experience_surfaces", {}),
        "showcase_portfolio_contract_id": portfolio.get("contract_id"),
        "guided_walkthrough_contract_id": walkthrough.get("contract_id"),
    }
    dump_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0


def action_materialize_project_template(rest: list[str]) -> int:
    return run([sys.executable, str(PROJECT_TEMPLATE_MATERIALIZER_PY), *rest])


def action_validate_runnable_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BONUS_EXPERIENCE_E2E_PY)])


def action_validate_runnable_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_FOUNDATION_E2E_PY)])


def action_lint_spec(_: list[str]) -> int:
    return run([sys.executable, str(SPEC_LINT_PY)])


def action_lint(_: list[str]) -> int:
    return run_steps(["check-source-hygiene-hard-cutover", "check-task-hygiene", "build-site", "check-markdown"])


def action_test_default(_: list[str]) -> int:
    return run_steps(["test-fast"])


def to_repo_relative(raw_path: str) -> str:
    candidate = Path(raw_path.strip())
    try:
        if candidate.is_absolute():
            return str(candidate.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return raw_path.strip()
    return raw_path.strip().replace("\\", "/")




def load_surface_from_report(
    steps: Sequence[dict[str, object]], surface_key: str
) -> dict[str, object] | None:
    for step in steps:
        report_paths = step.get("report_paths", [])
        if not isinstance(report_paths, list):
            continue
        for raw_path in report_paths:
            if not isinstance(raw_path, str):
                continue
            candidate = ROOT / raw_path
            if not candidate.is_file():
                continue
            try:
                payload = json.loads(candidate.read_text(encoding="utf-8"))
            except json.JSONDecodeError:
                continue
            surface = payload.get(surface_key)
            if isinstance(surface, dict):
                return surface
    return None


def safe_float(value: object, default: float = 0.0) -> float:
    if isinstance(value, (int, float)):
        return float(value)
    if isinstance(value, str):
        try:
            return float(value)
        except ValueError:
            return default
    return default


def load_json_report(raw_path: str) -> dict[str, object] | None:
    candidate = ROOT / raw_path
    if not candidate.is_file():
        return None
    try:
        payload = json.loads(candidate.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    if isinstance(payload, dict):
        return payload
    return None


def load_child_reports(
    steps: Sequence[dict[str, object]]
) -> list[dict[str, object]]:
    reports: list[dict[str, object]] = []
    for step in steps:
        report_paths = step.get("report_paths", [])
        if not isinstance(report_paths, list):
            continue
        for raw_path in report_paths:
            if not isinstance(raw_path, str):
                continue
            payload = load_json_report(raw_path)
            if payload is None:
                continue
            reports.append(
                {
                    "step_action": str(step.get("action", "")),
                    "path": raw_path,
                    "payload": payload,
                    "report_reused": bool(step.get("report_reused", False)),
                    "step_duration_seconds": safe_float(
                        step.get("duration_seconds", 0.0)
                    ),
                }
            )
    return reports


def classify_runtime_command(command: object) -> str:
    text = str(command).replace("\\", "/").lower()
    if "objc3c_native_compile.ps1" in text:
        return "wrapper"
    if "objc3c-native" in text:
        return "native"
    if "clang++" in text:
        return "clang++"
    if ".exe" in text:
        return "probe"
    return "other"


def summarize_runtime_acceptance_report(
    path: str, payload: dict[str, object], *, report_reused: bool
) -> dict[str, object]:
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    command_timings = timing_payload.get("command_timings", [])
    command_groups: dict[str, dict[str, object]] = {}
    if isinstance(command_timings, list):
        for entry in command_timings:
            if not isinstance(entry, dict):
                continue
            group = classify_runtime_command(entry.get("command", ""))
            bucket = command_groups.setdefault(
                group,
                {"count": 0, "duration_seconds": 0.0},
            )
            bucket["count"] = int(bucket["count"]) + 1
            bucket["duration_seconds"] = round(
                safe_float(bucket["duration_seconds"])
                + safe_float(entry.get("duration_seconds", 0.0)),
                6,
            )
    return {
        "report_path": path,
        "report_reused": report_reused,
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
        "case_count": payload.get("case_count")
        or timing_payload.get("total_case_count"),
        "completed_case_count": timing_payload.get("completed_case_count"),
        "command_count": len(command_timings)
        if isinstance(command_timings, list)
        else None,
        "default_compile_backend": payload.get("default_compile_backend"),
        "direct_compile_backend": payload.get("direct_compile_backend"),
        "wrapper_compile_backend": payload.get("wrapper_compile_backend"),
        "command_groups": command_groups,
        "slowest_cases": timing_payload.get("slowest_cases", []),
        "slowest_commands": timing_payload.get("slowest_commands", []),
    }


def summarize_execution_smoke_report(
    path: str, payload: dict[str, object]
) -> dict[str, object]:
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    selection = payload.get("selection", {})
    selection_payload = selection if isinstance(selection, dict) else {}
    return {
        "report_path": path,
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
        "status": payload.get("status"),
        "total": payload.get("total"),
        "passed": payload.get("passed"),
        "failed": payload.get("failed"),
        "selection": selection_payload,
        "stage_totals": timing_payload.get("stage_totals", {}),
        "slowest_fixtures": timing_payload.get("slowest_fixtures", []),
    }


def summarize_execution_replay_report(
    path: str, payload: dict[str, object]
) -> dict[str, object]:
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    return {
        "report_path": path,
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
        "status": payload.get("status"),
        "proof_run_id": payload.get("proof_run_id"),
        "selection": payload.get("selection", {}),
        "stage_totals": timing_payload.get("stage_totals", {}),
        "slowest_cases": timing_payload.get("slowest_cases", []),
    }


def validation_speed_budget_mode() -> str:
    budget_mode = os.environ.get(
        "OBJC3C_VALIDATION_SPEED_BUDGET_MODE",
        "warn",
    ).strip().lower()
    if budget_mode not in {"warn", "fail"}:
        return "warn"
    return budget_mode


def validation_speed_budgets(
    runtime_acceptance: dict[str, object] | None,
    execution_smoke: dict[str, object] | None,
    execution_replay: dict[str, object] | None,
    total_seconds: float,
) -> list[dict[str, object]]:
    budget_mode = validation_speed_budget_mode()
    budgets = [
        {
            "name": "runtime_acceptance_elapsed_seconds",
            "threshold_seconds": 60.0,
            "actual_seconds": safe_float(
                runtime_acceptance.get("elapsed_seconds") if runtime_acceptance else None
            ),
        },
        {
            "name": "execution_smoke_elapsed_seconds",
            "threshold_seconds": 90.0,
            "actual_seconds": safe_float(
                execution_smoke.get("elapsed_seconds") if execution_smoke else None
            ),
        },
        {
            "name": "execution_replay_elapsed_seconds",
            "threshold_seconds": 30.0,
            "actual_seconds": safe_float(
                execution_replay.get("elapsed_seconds") if execution_replay else None
            ),
        },
        {
            "name": "composite_elapsed_seconds",
            "threshold_seconds": 120.0,
            "actual_seconds": total_seconds,
        },
    ]
    for budget in budgets:
        actual = safe_float(budget["actual_seconds"])
        threshold = safe_float(budget["threshold_seconds"])
        if actual == 0.0:
            budget["status"] = "UNKNOWN"
        elif actual <= threshold:
            budget["status"] = "PASS"
        else:
            budget["status"] = "WARN"
        budget["mode"] = "fail" if budget_mode == "fail" else "warning-only"
    if runtime_acceptance is not None:
        command_groups = runtime_acceptance.get("command_groups", {})
        wrapper_count = None
        if isinstance(command_groups, dict):
            wrapper = command_groups.get("wrapper", {})
            if isinstance(wrapper, dict):
                wrapper_count = wrapper.get("count")
        budgets.append(
            {
                "name": "runtime_acceptance_wrapper_invocations",
                "threshold_count": 1,
                "actual_count": wrapper_count,
                "status": "PASS"
                if isinstance(wrapper_count, int) and wrapper_count <= 1
                else "UNKNOWN"
                if wrapper_count is None
                else "WARN",
                "mode": "fail" if budget_mode == "fail" else "warning-only",
            }
        )
    return budgets


def validation_budget_violations(
    budgets: Sequence[dict[str, object]],
) -> list[dict[str, object]]:
    return [
        budget
        for budget in budgets
        if budget.get("mode") == "fail" and budget.get("status") == "WARN"
    ]


def collect_child_timing(
    steps: Sequence[dict[str, object]]
) -> dict[str, object]:
    child_reports = load_child_reports(steps)
    runtime_acceptance: dict[str, object] | None = None
    execution_smoke: dict[str, object] | None = None
    execution_replay: dict[str, object] | None = None
    for report in child_reports:
        payload = report["payload"]
        if not isinstance(payload, dict):
            continue
        path = str(report["path"])
        if "default_compile_backend" in payload or "case_count" in payload:
            runtime_acceptance = summarize_runtime_acceptance_report(
                path,
                payload,
                report_reused=bool(report.get("report_reused", False)),
            )
        elif "proof_run_id" in payload:
            execution_replay = summarize_execution_replay_report(path, payload)
        elif "compile_command" in payload and "results" in payload:
            execution_smoke = summarize_execution_smoke_report(path, payload)
    total_step_seconds = round(
        sum(safe_float(step.get("duration_seconds", 0.0)) for step in steps),
        6,
    )
    estimated_no_skip_seconds = total_step_seconds
    if runtime_acceptance is not None and runtime_acceptance.get("report_reused"):
        for step in steps:
            if step.get("action") == "test-runtime-acceptance":
                estimated_no_skip_seconds -= safe_float(step.get("duration_seconds", 0.0))
                estimated_no_skip_seconds += safe_float(
                    runtime_acceptance.get("elapsed_seconds", 0.0)
                )
                break
    estimated_no_skip_seconds = round(estimated_no_skip_seconds, 6)
    return {
        "child_report_paths": [str(report["path"]) for report in child_reports],
        "runtime_acceptance": runtime_acceptance,
        "execution_smoke": execution_smoke,
        "execution_replay": execution_replay,
        "estimated_no_skip_seconds": estimated_no_skip_seconds,
        "budgets": validation_speed_budgets(
            runtime_acceptance,
            execution_smoke,
            execution_replay,
            estimated_no_skip_seconds,
        ),
    }


def write_composite_validation_report(
    action: str,
    steps: list[dict[str, object]],
    *,
    status: str,
) -> Path:
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    report_path = PUBLIC_WORKFLOW_REPORT_ROOT / f"{action}.json"
    child_timing = collect_child_timing(steps)
    budgets = child_timing.get("budgets", [])
    budget_violations = (
        validation_budget_violations(budgets)
        if isinstance(budgets, list)
        else []
    )
    effective_status = (
        "FAIL"
        if status == "PASS" and budget_violations
        else status
    )
    payload = {
        "action": action,
        "status": effective_status,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "timing": {
            "step_count": len(steps),
            "total_step_duration_seconds": round(
                sum(float(step.get("duration_seconds", 0.0)) for step in steps),
                6,
            ),
            "slowest_steps": sorted(
                [
                    {
                        "action": str(step.get("action", "")),
                        "duration_seconds": float(step.get("duration_seconds", 0.0)),
                        "exit_code": int(step.get("exit_code", 0)),
                    }
                    for step in steps
                ],
                key=lambda entry: entry["duration_seconds"],
                reverse=True,
            )[:10],
            "estimated_no_skip_seconds": child_timing["estimated_no_skip_seconds"],
        },
        "child_timing": child_timing,
        "budget_policy": {
            "contract_id": "objc3c.validation.speed.budget.policy.v1",
            "mode": (
                "fail"
                if validation_speed_budget_mode() == "fail"
                else "warning-only"
            ),
            "violations": budget_violations,
            "fail_closed": True,
        },
        "claim_boundary": {
            "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
            "reports_are_authoritative_only_when_child_steps_are_compile-coupled": True,
            "authoritative_child_surfaces": [
                "scripts/check_objc3c_runtime_acceptance.py",
                "scripts/check_objc3c_execution_replay_proof.ps1",
                "scripts/check_objc3c_native_execution_smoke.ps1",
            ],
            "non_authoritative_inputs": [
                "integrated report paths by themselves",
                "sidecar-only summaries with no matching emitted object/probe path",
                "synthetic or hand-authored llvm ir used without coupled compile output",
            ],
        },
        "steps": steps,
    }
    runtime_state_publication_surface = load_surface_from_report(
        steps, "runtime_state_publication_surface"
    )
    if runtime_state_publication_surface is not None:
        payload["runtime_state_publication_surface"] = runtime_state_publication_surface
    runtime_error_execution_cleanup_source_surface = load_surface_from_report(
        steps, "runtime_error_execution_cleanup_source_surface"
    )
    if runtime_error_execution_cleanup_source_surface is not None:
        payload["runtime_error_execution_cleanup_source_surface"] = (
            runtime_error_execution_cleanup_source_surface
        )
    runtime_catch_filter_finalization_source_surface = load_surface_from_report(
        steps, "runtime_catch_filter_finalization_source_surface"
    )
    if runtime_catch_filter_finalization_source_surface is not None:
        payload["runtime_catch_filter_finalization_source_surface"] = (
            runtime_catch_filter_finalization_source_surface
        )
    runtime_error_propagation_cleanup_semantics_surface = load_surface_from_report(
        steps, "runtime_error_propagation_cleanup_semantics_surface"
    )
    if runtime_error_propagation_cleanup_semantics_surface is not None:
        payload["runtime_error_propagation_cleanup_semantics_surface"] = (
            runtime_error_propagation_cleanup_semantics_surface
        )
    runtime_bridging_filter_unwind_diagnostics_surface = load_surface_from_report(
        steps, "runtime_bridging_filter_unwind_diagnostics_surface"
    )
    if runtime_bridging_filter_unwind_diagnostics_surface is not None:
        payload["runtime_bridging_filter_unwind_diagnostics_surface"] = (
            runtime_bridging_filter_unwind_diagnostics_surface
        )
    runtime_error_lowering_unwind_bridge_helper_surface = load_surface_from_report(
        steps, "runtime_error_lowering_unwind_bridge_helper_surface"
    )
    if runtime_error_lowering_unwind_bridge_helper_surface is not None:
        payload["runtime_error_lowering_unwind_bridge_helper_surface"] = (
            runtime_error_lowering_unwind_bridge_helper_surface
        )
    runtime_error_runtime_abi_cleanup_surface = load_surface_from_report(
        steps, "runtime_error_runtime_abi_cleanup_surface"
    )
    if runtime_error_runtime_abi_cleanup_surface is not None:
        payload["runtime_error_runtime_abi_cleanup_surface"] = (
            runtime_error_runtime_abi_cleanup_surface
        )
    runtime_error_propagation_catch_cleanup_runtime_implementation_surface = (
        load_surface_from_report(
            steps, "runtime_error_propagation_catch_cleanup_runtime_implementation_surface"
        )
    )
    if runtime_error_propagation_catch_cleanup_runtime_implementation_surface is not None:
        payload["runtime_error_propagation_catch_cleanup_runtime_implementation_surface"] = (
            runtime_error_propagation_catch_cleanup_runtime_implementation_surface
        )
    acceptance_suite_surface = load_surface_from_report(steps, "acceptance_suite_surface")
    if acceptance_suite_surface is not None:
        payload["acceptance_suite_surface"] = acceptance_suite_surface
    runtime_installation_abi_surface = load_surface_from_report(
        steps, "runtime_installation_abi_surface"
    )
    if runtime_installation_abi_surface is not None:
        payload["runtime_installation_abi_surface"] = runtime_installation_abi_surface
    runtime_loader_lifecycle_surface = load_surface_from_report(
        steps, "runtime_loader_lifecycle_surface"
    )
    if runtime_loader_lifecycle_surface is not None:
        payload["runtime_loader_lifecycle_surface"] = runtime_loader_lifecycle_surface
    runtime_object_model_realization_source_surface = load_surface_from_report(
        steps, "runtime_object_model_realization_source_surface"
    )
    if runtime_object_model_realization_source_surface is not None:
        payload["runtime_object_model_realization_source_surface"] = (
            runtime_object_model_realization_source_surface
        )
    runtime_block_arc_unified_source_surface = load_surface_from_report(
        steps, "runtime_block_arc_unified_source_surface"
    )
    if runtime_block_arc_unified_source_surface is not None:
        payload["runtime_block_arc_unified_source_surface"] = (
            runtime_block_arc_unified_source_surface
        )
    runtime_ownership_transfer_capture_family_source_surface = load_surface_from_report(
        steps, "runtime_ownership_transfer_capture_family_source_surface"
    )
    if runtime_ownership_transfer_capture_family_source_surface is not None:
        payload["runtime_ownership_transfer_capture_family_source_surface"] = (
            runtime_ownership_transfer_capture_family_source_surface
        )
    runtime_block_arc_lowering_helper_surface = load_surface_from_report(
        steps, "runtime_block_arc_lowering_helper_surface"
    )
    if runtime_block_arc_lowering_helper_surface is not None:
        payload["runtime_block_arc_lowering_helper_surface"] = (
            runtime_block_arc_lowering_helper_surface
        )
    runtime_block_arc_runtime_abi_surface = load_surface_from_report(
        steps, "runtime_block_arc_runtime_abi_surface"
    )
    if runtime_block_arc_runtime_abi_surface is not None:
        payload["runtime_block_arc_runtime_abi_surface"] = (
            runtime_block_arc_runtime_abi_surface
        )
    runtime_property_ivar_storage_accessor_source_surface = load_surface_from_report(
        steps, "runtime_property_ivar_storage_accessor_source_surface"
    )
    if runtime_property_ivar_storage_accessor_source_surface is not None:
        payload["runtime_property_ivar_storage_accessor_source_surface"] = (
            runtime_property_ivar_storage_accessor_source_surface
        )
    storage_accessor_runtime_abi_surface = load_surface_from_report(
        steps, "storage_accessor_runtime_abi_surface"
    )
    if storage_accessor_runtime_abi_surface is not None:
        payload["storage_accessor_runtime_abi_surface"] = (
            storage_accessor_runtime_abi_surface
        )
    runtime_property_ivar_accessor_reflection_implementation_surface = (
        load_surface_from_report(
            steps,
            "runtime_property_ivar_accessor_reflection_implementation_surface",
        )
    )
    if runtime_property_ivar_accessor_reflection_implementation_surface is not None:
        payload["runtime_property_ivar_accessor_reflection_implementation_surface"] = (
            runtime_property_ivar_accessor_reflection_implementation_surface
        )
    runtime_claimable_surface_residual_non_claimable_gaps_source_surface = load_surface_from_report(
        steps, "runtime_claimable_surface_residual_non_claimable_gaps_source_surface"
    )
    if runtime_claimable_surface_residual_non_claimable_gaps_source_surface is not None:
        payload["runtime_claimable_surface_residual_non_claimable_gaps_source_surface"] = (
            runtime_claimable_surface_residual_non_claimable_gaps_source_surface
        )
    runtime_strict_profile_feature_claim_source_surface = load_surface_from_report(
        steps, "runtime_strict_profile_feature_claim_source_surface"
    )
    if runtime_strict_profile_feature_claim_source_surface is not None:
        payload["runtime_strict_profile_feature_claim_source_surface"] = (
            runtime_strict_profile_feature_claim_source_surface
        )
    runtime_claimability_semantics_release_policy_surface = load_surface_from_report(
        steps, "runtime_claimability_semantics_release_policy_surface"
    )
    if runtime_claimability_semantics_release_policy_surface is not None:
        payload["runtime_claimability_semantics_release_policy_surface"] = (
            runtime_claimability_semantics_release_policy_surface
        )
    runtime_strict_profile_claim_implementation_surface = load_surface_from_report(
        steps, "runtime_strict_profile_claim_implementation_surface"
    )
    if runtime_strict_profile_claim_implementation_surface is not None:
        payload["runtime_strict_profile_claim_implementation_surface"] = (
            runtime_strict_profile_claim_implementation_surface
        )
    runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface = load_surface_from_report(
        steps, "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface"
    )
    if runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface is not None:
        payload["runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface"] = (
            runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface
        )
    runtime_claim_publication_dashboard_schema_surface = load_surface_from_report(
        steps, "runtime_claim_publication_dashboard_schema_surface"
    )
    if runtime_claim_publication_dashboard_schema_surface is not None:
        payload["runtime_claim_publication_dashboard_schema_surface"] = (
            runtime_claim_publication_dashboard_schema_surface
        )
    runtime_final_claim_publication_deprecated_path_shutdown_surface = load_surface_from_report(
        steps, "runtime_final_claim_publication_deprecated_path_shutdown_surface"
    )
    if runtime_final_claim_publication_deprecated_path_shutdown_surface is not None:
        payload["runtime_final_claim_publication_deprecated_path_shutdown_surface"] = (
            runtime_final_claim_publication_deprecated_path_shutdown_surface
        )
    runtime_release_candidate_claim_abi_surface = load_surface_from_report(
        steps, "runtime_release_candidate_claim_abi_surface"
    )
    if runtime_release_candidate_claim_abi_surface is not None:
        payload["runtime_release_candidate_claim_abi_surface"] = (
            runtime_release_candidate_claim_abi_surface
        )
    runtime_final_release_evidence_descaffolding_implementation_surface = load_surface_from_report(
        steps, "runtime_final_release_evidence_descaffolding_implementation_surface"
    )
    if runtime_final_release_evidence_descaffolding_implementation_surface is not None:
        payload["runtime_final_release_evidence_descaffolding_implementation_surface"] = (
            runtime_final_release_evidence_descaffolding_implementation_surface
        )
    runtime_metaprogramming_source_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_source_surface"
    )
    if runtime_metaprogramming_source_surface is not None:
        payload["runtime_metaprogramming_source_surface"] = (
            runtime_metaprogramming_source_surface
        )
    runtime_metaprogramming_package_provenance_source_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_package_provenance_source_surface"
    )
    if runtime_metaprogramming_package_provenance_source_surface is not None:
        payload["runtime_metaprogramming_package_provenance_source_surface"] = (
            runtime_metaprogramming_package_provenance_source_surface
        )
    runtime_metaprogramming_semantics_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_semantics_surface"
    )
    if runtime_metaprogramming_semantics_surface is not None:
        payload["runtime_metaprogramming_semantics_surface"] = (
            runtime_metaprogramming_semantics_surface
        )
    runtime_metaprogramming_lowering_host_cache_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_lowering_host_cache_surface"
    )
    if runtime_metaprogramming_lowering_host_cache_surface is not None:
        payload["runtime_metaprogramming_lowering_host_cache_surface"] = (
            runtime_metaprogramming_lowering_host_cache_surface
        )
    runtime_cross_module_metaprogramming_artifact_preservation_surface = (
        load_surface_from_report(
            steps, "runtime_cross_module_metaprogramming_artifact_preservation_surface"
        )
    )
    if runtime_cross_module_metaprogramming_artifact_preservation_surface is not None:
        payload["runtime_cross_module_metaprogramming_artifact_preservation_surface"] = (
            runtime_cross_module_metaprogramming_artifact_preservation_surface
        )
    runtime_metaprogramming_runtime_abi_cache_surface = load_surface_from_report(
        steps, "runtime_metaprogramming_runtime_abi_cache_surface"
    )
    if runtime_metaprogramming_runtime_abi_cache_surface is not None:
        payload["runtime_metaprogramming_runtime_abi_cache_surface"] = (
            runtime_metaprogramming_runtime_abi_cache_surface
        )
    runtime_metaprogramming_cache_runtime_integration_implementation_surface = (
        load_surface_from_report(
            steps,
            "runtime_metaprogramming_cache_runtime_integration_implementation_surface",
        )
    )
    if runtime_metaprogramming_cache_runtime_integration_implementation_surface is not None:
        payload["runtime_metaprogramming_cache_runtime_integration_implementation_surface"] = (
            runtime_metaprogramming_cache_runtime_integration_implementation_surface
        )
    for surface_key in (
        "runtime_cross_module_package_interop_source_surface",
        "runtime_textual_binary_interface_parity_source_surface",
        "runtime_mixed_image_compatibility_interop_semantics_surface",
        "runtime_package_loading_module_identity_semantics_surface",
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface",
        "runtime_import_version_feature_claim_diagnostics_surface",
        "runtime_packaging_bridge_loader_artifact_surface",
        "runtime_mixed_image_package_lowering_bridge_emission_surface",
        "runtime_cross_language_replay_import_surface_preservation_surface",
        "runtime_package_loader_bridge_abi_surface",
        "runtime_package_loading_interop_implementation_surface",
    ):
        surface = load_surface_from_report(steps, surface_key)
        if surface is not None:
            payload[surface_key] = surface
    runtime_property_atomicity_synthesis_reflection_source_surface = load_surface_from_report(
        steps, "runtime_property_atomicity_synthesis_reflection_source_surface"
    )
    if runtime_property_atomicity_synthesis_reflection_source_surface is not None:
        payload["runtime_property_atomicity_synthesis_reflection_source_surface"] = (
            runtime_property_atomicity_synthesis_reflection_source_surface
        )
    runtime_realization_lowering_reflection_artifact_surface = load_surface_from_report(
        steps, "runtime_realization_lowering_reflection_artifact_surface"
    )
    if runtime_realization_lowering_reflection_artifact_surface is not None:
        payload["runtime_realization_lowering_reflection_artifact_surface"] = (
            runtime_realization_lowering_reflection_artifact_surface
        )
    runtime_dispatch_table_reflection_record_lowering_surface = load_surface_from_report(
        steps, "runtime_dispatch_table_reflection_record_lowering_surface"
    )
    if runtime_dispatch_table_reflection_record_lowering_surface is not None:
        payload["runtime_dispatch_table_reflection_record_lowering_surface"] = (
            runtime_dispatch_table_reflection_record_lowering_surface
        )
    runtime_cross_module_realized_metadata_replay_preservation_surface = load_surface_from_report(
        steps, "runtime_cross_module_realized_metadata_replay_preservation_surface"
    )
    if runtime_cross_module_realized_metadata_replay_preservation_surface is not None:
        payload["runtime_cross_module_realized_metadata_replay_preservation_surface"] = (
            runtime_cross_module_realized_metadata_replay_preservation_surface
        )
    runtime_object_model_abi_query_surface = load_surface_from_report(
        steps, "runtime_object_model_abi_query_surface"
    )
    if runtime_object_model_abi_query_surface is not None:
        payload["runtime_object_model_abi_query_surface"] = (
            runtime_object_model_abi_query_surface
        )
    runtime_realization_lookup_reflection_implementation_surface = load_surface_from_report(
        steps, "runtime_realization_lookup_reflection_implementation_surface"
    )
    if runtime_realization_lookup_reflection_implementation_surface is not None:
        payload["runtime_realization_lookup_reflection_implementation_surface"] = (
            runtime_realization_lookup_reflection_implementation_surface
        )
    runtime_reflection_query_surface = load_surface_from_report(
        steps, "runtime_reflection_query_surface"
    )
    if runtime_reflection_query_surface is not None:
        payload["runtime_reflection_query_surface"] = runtime_reflection_query_surface
    runtime_realization_lookup_semantics_surface = load_surface_from_report(
        steps, "runtime_realization_lookup_semantics_surface"
    )
    if runtime_realization_lookup_semantics_surface is not None:
        payload["runtime_realization_lookup_semantics_surface"] = (
            runtime_realization_lookup_semantics_surface
        )
    runtime_class_metaclass_protocol_realization_surface = load_surface_from_report(
        steps, "runtime_class_metaclass_protocol_realization_surface"
    )
    if runtime_class_metaclass_protocol_realization_surface is not None:
        payload["runtime_class_metaclass_protocol_realization_surface"] = (
            runtime_class_metaclass_protocol_realization_surface
        )
    runtime_category_attachment_merged_dispatch_surface = load_surface_from_report(
        steps, "runtime_category_attachment_merged_dispatch_surface"
    )
    if runtime_category_attachment_merged_dispatch_surface is not None:
        payload["runtime_category_attachment_merged_dispatch_surface"] = (
            runtime_category_attachment_merged_dispatch_surface
        )
    runtime_reflection_visibility_coherence_diagnostics_surface = load_surface_from_report(
        steps, "runtime_reflection_visibility_coherence_diagnostics_surface"
    )
    if runtime_reflection_visibility_coherence_diagnostics_surface is not None:
        payload["runtime_reflection_visibility_coherence_diagnostics_surface"] = (
            runtime_reflection_visibility_coherence_diagnostics_surface
        )
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return report_path


def run_composite_step(action: str, command: Sequence[str]) -> dict[str, object]:
    started_at = perf_counter()
    if (
        action.startswith("test-runtime-acceptance")
        and os.environ.get("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN") == "1"
    ):
        return {
            "action": action,
            "command": [str(token) for token in command],
            "exit_code": 0,
            "report_paths": ["tmp/reports/runtime/acceptance/summary.json"],
            "report_reused": True,
            "report_reuse_source": "OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN",
            "duration_seconds": round(perf_counter() - started_at, 6),
        }
    normalized = [str(token) for token in command]
    if (
        len(normalized) >= 3
        and Path(normalized[0]).resolve() == Path(sys.executable).resolve()
        and Path(normalized[1]).resolve() == Path(__file__).resolve()
    ):
        nested_action = normalized[2]
        nested_rest = normalized[3:]
        exit_code = execute_registered_action(nested_action, nested_rest)
        return {
            "action": action,
            "command": normalized,
            "exit_code": exit_code,
            "report_paths": [],
            "executed_in_process": True,
            "duration_seconds": round(perf_counter() - started_at, 6),
        }
    result = run_capture(command)
    return {
        "action": action,
        "command": normalized,
        "exit_code": result.returncode,
        "report_paths": extract_report_paths(result.stdout),
        "duration_seconds": round(perf_counter() - started_at, 6),
    }


def run_composite_validation(action: str, steps: list[tuple[str, Sequence[str]]]) -> int:
    results: list[dict[str, object]] = []
    workflow_started_at = perf_counter()
    for index, (step_action, command) in enumerate(steps, start=1):
        previous = str(results[-1]["action"]) if results else "none"
        print(
            f"public-workflow-progress: [{index}/{len(steps)}] START action={step_action} "
            f"elapsed={perf_counter() - workflow_started_at:.3f}s last={previous}",
            flush=True,
        )
        step = run_composite_step(step_action, command)
        results.append(step)
        print(
            f"public-workflow-progress: [{index}/{len(steps)}] DONE action={step_action} "
            f"duration={float(step.get('duration_seconds', 0.0)):.3f}s "
            f"elapsed={perf_counter() - workflow_started_at:.3f}s exit={step['exit_code']}",
            flush=True,
        )
        if step["exit_code"] != 0:
            report_path = write_composite_validation_report(action, results, status="FAIL")
            print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
            return int(step["exit_code"])
    report_path = write_composite_validation_report(action, results, status="PASS")
    print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
    report_payload = load_latest_report_payload(report_path)
    if isinstance(report_payload, dict) and report_payload.get("status") != "PASS":
        return 1
    return 0


def action_test_fast(_: list[str]) -> int:
    return run_composite_validation(
        "test-fast",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1), "-Limit", "12"]),
            ("test-runtime-acceptance-fast", [sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"]),
            ("test-execution-replay-focused", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1), "-Limit", "1"]),
        ],
    )


def action_test_smoke(_: list[str]) -> int:
    return run_steps(["test-execution-smoke"])


def action_test_ci(_: list[str]) -> int:
    return run_composite_validation(
        "test-ci",
        [
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("validate-developer-tooling", [sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)]),
            ("validate-bonus-experiences", [sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)]),
            ("validate-stdlib-foundation", [sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)]),
            ("validate-stdlib-advanced", [sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)]),
            ("validate-stdlib-program", [sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)]),
            ("validate-performance-governance", workflow_command("validate-performance-governance")),
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
        ],
    )


def action_test_recovery(rest: list[str]) -> int:
    return pwsh_file(RECOVERY_PS1, *rest)


def action_test_execution_smoke(rest: list[str]) -> int:
    return pwsh_file(SMOKE_PS1, *rest)


def action_test_execution_replay(rest: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, *rest)


def action_test_execution_replay_focused(_: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, "-Limit", "1")


def action_test_runtime_acceptance(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY)])


def action_test_runtime_acceptance_fast(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"])


def action_test_runtime_acceptance_diagnostics(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "diagnostics"])


def action_test_runtime_acceptance_cross_module(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "cross-module"])


def action_test_runtime_acceptance_block_arc(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "block-arc"])


def action_test_runtime_acceptance_concurrency(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "concurrency"])


def action_test_compile_wrapper_self_audit(_: list[str]) -> int:
    return run([sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)])


def action_proof_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_PROOF_PACKET_PY)])


def action_validate_runtime_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_runnable_bootstrap(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BOOTSTRAP_E2E_PY)])


def action_validate_block_arc_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BLOCK_ARC_CONFORMANCE_PY)])


def action_validate_runnable_block_arc(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BLOCK_ARC_E2E_PY)])


def action_validate_concurrency_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONCURRENCY_CONFORMANCE_PY)])


def action_validate_runnable_concurrency(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONCURRENCY_E2E_PY)])


def action_validate_object_model_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY)])


def action_validate_runnable_object_model(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_OBJECT_MODEL_E2E_PY)])


def action_validate_storage_reflection_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY)])


def action_validate_runnable_storage_reflection(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STORAGE_REFLECTION_E2E_PY)])


def action_validate_error_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_ERROR_CONFORMANCE_PY)])


def action_validate_runnable_error(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_ERROR_E2E_PY)])


def action_validate_interop_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_INTEROP_CONFORMANCE_PY)])


def action_validate_runnable_interop(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_INTEROP_E2E_PY)])


def action_validate_metaprogramming_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY)])


def action_validate_runnable_metaprogramming(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_METAPROGRAMMING_E2E_PY)])


def action_validate_release_candidate_conformance(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY)])


def action_validate_runnable_release_candidate(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_RELEASE_CANDIDATE_E2E_PY)])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)


def action_benchmark_compiler_throughput(rest: list[str]) -> int:
    return pwsh_file(COMPILER_THROUGHPUT_PS1, *rest)


def action_validate_compiler_throughput(_: list[str]) -> int:
    return run([sys.executable, str(COMPILER_THROUGHPUT_INTEGRATION_PY)])


def action_validate_runnable_compiler_throughput(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_COMPILER_THROUGHPUT_E2E_PY)])


VALIDATION_PROFILE_RULES: dict[str, dict[str, object]] = {
    "docs": {
        "path_prefixes": ("docs/", "site/", "README", "CHANGELOG", "package.json"),
        "recommended_actions": (
            "check-documentation-surface",
            "check-markdown",
            "check-public-command-surface",
        ),
        "exhaustive_actions": ("validate-documentation-surface",),
        "skipped_by_default": (
            "runtime acceptance",
            "execution smoke",
            "execution replay",
        ),
    },
    "lowering": {
        "path_prefixes": (
            "native/objc3c/src/ir/",
            "native/objc3c/src/sema/",
            "native/objc3c/src/parser/",
            "tests/tooling/fixtures/native/",
        ),
        "recommended_actions": (
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-diagnostics",
            "test-execution-replay-focused",
            "test-execution-smoke -- -Limit 12",
        ),
        "exhaustive_actions": ("test-full", "test-nightly"),
        "skipped_by_default": ("full smoke matrix", "nightly recovery fan-out"),
    },
    "runtime": {
        "path_prefixes": (
            "native/objc3c/src/runtime/",
            "tests/tooling/runtime/",
            "native/objc3c/runtime/",
        ),
        "recommended_actions": (
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-block-arc",
            "test-runtime-acceptance-concurrency",
            "test-execution-smoke",
            "test-execution-replay-focused",
        ),
        "exhaustive_actions": ("test-runtime-acceptance", "test-nightly"),
        "skipped_by_default": ("release packaging validations",),
    },
    "diagnostics": {
        "path_prefixes": (
            "tests/tooling/fixtures/native/negative/",
            "tests/tooling/fixtures/native/diagnostics/",
            "native/objc3c/src/diagnostics/",
        ),
        "recommended_actions": (
            "test-negative-expectations",
            "test-runtime-acceptance-diagnostics",
        ),
        "exhaustive_actions": ("test-runtime-acceptance", "test-nightly"),
        "skipped_by_default": ("runtime-only smoke cases not touching diagnostics"),
    },
    "conformance": {
        "path_prefixes": (
            "tests/conformance/",
            "docs/objc3c-native/src/",
            "scripts/check_objc3c_runnable_",
        ),
        "recommended_actions": (
            "validate-conformance-corpus",
            "validate-runtime-architecture",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("stress and fuzz validation"),
    },
    "stress": {
        "path_prefixes": (
            "tests/tooling/fixtures/stress/",
            "scripts/run_objc3c_fuzz",
            "scripts/run_objc3c_lowering_runtime_stress",
            "scripts/run_objc3c_mixed_module_differential",
            "scripts/run_objc3c_stress",
        ),
        "recommended_actions": (
            "validate-stress",
            "test-fuzz-safety",
            "test-lowering-runtime-stress",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("docs-only validation"),
    },
    "release-claim": {
        "path_prefixes": (
            "docs/runbooks/",
            "schemas/",
            "release/",
            "scripts/publish_",
            "scripts/build_objc3c_release",
        ),
        "recommended_actions": (
            "check-release-evidence",
            "validate-release-foundation",
            "validate-public-conformance-reporting",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("local-only playground inspections"),
    },
}


def latest_json_file(root: Path) -> Path | None:
    if root.is_file():
        return root
    if not root.exists():
        return None
    candidates = sorted(
        (candidate for candidate in root.rglob("*.json") if candidate.is_file()),
        key=lambda candidate: candidate.stat().st_mtime,
        reverse=True,
    )
    return candidates[0] if candidates else None


def relative_path_or_none(path: Path | None) -> str | None:
    if path is None:
        return None
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


def git_changed_paths() -> list[str]:
    commands = (
        ["git", "diff", "--name-only", "HEAD", "--"],
        ["git", "ls-files", "--others", "--exclude-standard"],
    )
    changed: list[str] = []
    for command in commands:
        result = subprocess.run(
            command,
            cwd=ROOT,
            check=False,
            text=True,
            capture_output=True,
        )
        if result.returncode != 0:
            continue
        changed.extend(
            line.strip().replace("\\", "/")
            for line in result.stdout.splitlines()
            if line.strip()
        )
    return sorted(dict.fromkeys(changed))


def select_validation_profiles(paths: Sequence[str]) -> dict[str, object]:
    matched_profiles: list[dict[str, object]] = []
    for profile_name, rule in VALIDATION_PROFILE_RULES.items():
        prefixes = rule.get("path_prefixes", ())
        matched_paths = [
            path
            for path in paths
            if any(path.startswith(str(prefix)) for prefix in prefixes)
        ]
        if not matched_paths:
            continue
        matched_profiles.append(
            {
                "profile": profile_name,
                "matched_paths": matched_paths[:20],
                "recommended_actions": list(rule.get("recommended_actions", ())),
                "exhaustive_actions": list(rule.get("exhaustive_actions", ())),
                "skipped_by_default": list(rule.get("skipped_by_default", ())),
            }
        )
    if not matched_profiles and paths:
        matched_profiles.append(
            {
                "profile": "repo",
                "matched_paths": list(paths)[:20],
                "recommended_actions": ("test-fast", "check-task-hygiene"),
                "exhaustive_actions": ("test-full", "test-nightly"),
                "skipped_by_default": ("nightly release and stress fan-out",),
            }
        )
    return {
        "changed_paths": list(paths),
        "profiles": matched_profiles,
        "manual_override": {
            "fast": f"{WORKFLOW_COMMAND_TEXT} test-fast",
            "full": f"{WORKFLOW_COMMAND_TEXT} test-full",
            "nightly": f"{WORKFLOW_COMMAND_TEXT} test-nightly",
        },
    }


def load_latest_report_payload(path: Path | None) -> dict[str, object] | None:
    if path is None:
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    return payload if isinstance(payload, dict) else None


def dashboard_section_from_report(
    label: str, path: Path | None, payload: dict[str, object] | None
) -> dict[str, object]:
    if payload is None:
        return {
            "label": label,
            "report_path": relative_path_or_none(path),
            "status": "MISSING",
        }
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    section: dict[str, object] = {
        "label": label,
        "report_path": relative_path_or_none(path),
        "status": payload.get("status", "UNKNOWN"),
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
    }
    if "case_count" in payload or "default_compile_backend" in payload:
        section.update(
            summarize_runtime_acceptance_report(
                relative_path_or_none(path) or "",
                payload,
                report_reused=False,
            )
        )
    elif "proof_run_id" in payload:
        section.update(
            summarize_execution_replay_report(relative_path_or_none(path) or "", payload)
        )
    elif "compile_command" in payload and "results" in payload:
        section.update(
            summarize_execution_smoke_report(relative_path_or_none(path) or "", payload)
        )
    elif "child_timing" in payload:
        section["child_timing"] = payload.get("child_timing")
        section["slowest_steps"] = timing_payload.get("slowest_steps", [])
        section["estimated_no_skip_seconds"] = timing_payload.get(
            "estimated_no_skip_seconds"
        )
    return section


def write_validation_timing_markdown(payload: dict[str, object], path: Path) -> None:
    lines = [
        "# ObjC3 Validation Timing Dashboard",
        "",
        f"- generated: `{payload['generated_at_utc']}`",
        "- contract: `objc3c.validation.speed.dashboard.v1`",
        "",
        "## Reports",
    ]
    reports = payload.get("reports", {})
    if isinstance(reports, dict):
        for name, report in reports.items():
            if not isinstance(report, dict):
                continue
            lines.append(
                f"- `{name}`: status=`{report.get('status')}` "
                f"elapsed=`{report.get('elapsed_seconds', 0.0)}` "
                f"path=`{report.get('report_path')}`"
            )
    lines.extend(["", "## Budget Status"])
    budgets = payload.get("budgets", [])
    if isinstance(budgets, list):
        for budget in budgets:
            if isinstance(budget, dict):
                lines.append(
                    f"- `{budget.get('name')}`: `{budget.get('status')}` "
                    f"actual=`{budget.get('actual_seconds', budget.get('actual_count'))}`"
                )
    lines.extend(["", "## Validation Profiles"])
    profiles = payload.get("validation_profiles", {})
    if isinstance(profiles, dict):
        for profile in profiles.get("profiles", []):
            if isinstance(profile, dict):
                actions = ", ".join(str(action) for action in profile.get("recommended_actions", []))
                lines.append(f"- `{profile.get('profile')}`: {actions}")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def action_inspect_validation_timing(_: list[str]) -> int:
    runtime_path = latest_json_file(ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json")
    smoke_path = latest_json_file(ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke")
    replay_path = latest_json_file(ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-replay-proof")
    test_full_path = latest_json_file(PUBLIC_WORKFLOW_REPORT_ROOT / "test-full.json")
    test_fast_path = latest_json_file(PUBLIC_WORKFLOW_REPORT_ROOT / "test-fast.json")
    report_payloads = {
        "test_full": load_latest_report_payload(test_full_path),
        "test_fast": load_latest_report_payload(test_fast_path),
        "runtime_acceptance": load_latest_report_payload(runtime_path),
        "execution_smoke": load_latest_report_payload(smoke_path),
        "execution_replay": load_latest_report_payload(replay_path),
    }
    reports = {
        "test_full": dashboard_section_from_report(
            "test_full", test_full_path, report_payloads["test_full"]
        ),
        "test_fast": dashboard_section_from_report(
            "test_fast", test_fast_path, report_payloads["test_fast"]
        ),
        "runtime_acceptance": dashboard_section_from_report(
            "runtime_acceptance",
            runtime_path,
            report_payloads["runtime_acceptance"],
        ),
        "execution_smoke": dashboard_section_from_report(
            "execution_smoke", smoke_path, report_payloads["execution_smoke"]
        ),
        "execution_replay": dashboard_section_from_report(
            "execution_replay", replay_path, report_payloads["execution_replay"]
        ),
    }
    runtime_report = reports["runtime_acceptance"]
    smoke_report = reports["execution_smoke"]
    replay_report = reports["execution_replay"]
    total_seconds = safe_float(
        reports["test_full"].get("estimated_no_skip_seconds")
        or reports["test_full"].get("elapsed_seconds")
        or reports["test_fast"].get("elapsed_seconds")
    )
    payload = {
        "contract_id": "objc3c.validation.speed.dashboard.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "reports": reports,
        "budgets": validation_speed_budgets(
            runtime_report if runtime_report.get("status") != "MISSING" else None,
            smoke_report if smoke_report.get("status") != "MISSING" else None,
            replay_report if replay_report.get("status") != "MISSING" else None,
            total_seconds,
        ),
        "validation_profiles": select_validation_profiles(git_changed_paths()),
        "profile_catalog": VALIDATION_PROFILE_RULES,
        "notes": [
            "This dashboard is generated from the latest local timing reports under tmp.",
            "Budget results are warning-only until stable post-optimization baselines are established.",
            "Generated reports are observability outputs; checked-in scripts remain the source of truth.",
        ],
    }
    PUBLIC_WORKFLOW_REPORT_ROOT.mkdir(parents=True, exist_ok=True)
    dashboard_path = PUBLIC_WORKFLOW_REPORT_ROOT / "validation-timing-dashboard.json"
    markdown_path = PUBLIC_WORKFLOW_REPORT_ROOT / "validation-timing-dashboard.md"
    dashboard_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    write_validation_timing_markdown(payload, markdown_path)
    print(f"summary_path: {dashboard_path.relative_to(ROOT).as_posix()}")
    print(f"dashboard_path: {markdown_path.relative_to(ROOT).as_posix()}")
    return 0


def action_test_full(_: list[str]) -> int:
    return run_composite_validation(
        "test-full",
        [
            ("test-compile-wrapper-self-audit", [sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)]),
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1), "-Limit", "24"]),
            ("test-runtime-acceptance-fast", [sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"]),
            ("test-execution-replay-focused", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1), "-Limit", "1"]),
        ],
    )


def action_test_nightly(_: list[str]) -> int:
    return run_composite_validation(
        "test-nightly",
        [
            ("test-execution-smoke", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SMOKE_PS1)]),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            ("test-execution-replay", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1)]),
            ("validate-conformance-corpus", [sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)]),
            ("validate-stress", workflow_command("validate-stress")),
            ("validate-external-validation", workflow_command("validate-external-validation")),
            ("validate-public-conformance-reporting", workflow_command("validate-public-conformance-reporting")),
            ("validate-performance-governance", workflow_command("validate-performance-governance")),
            ("validate-release-foundation", workflow_command("validate-release-foundation")),
            ("validate-packaging-channels", workflow_command("validate-packaging-channels")),
            ("validate-release-operations", workflow_command("validate-release-operations")),
            ("validate-distribution-credibility", workflow_command("validate-distribution-credibility")),
            ("test-recovery", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(RECOVERY_PS1)]),
            ("test-fixture-matrix", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(MATRIX_PS1)]),
            ("test-negative-expectations", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(NEGATIVE_EXPECTATIONS_PS1)]),
        ],
    )


def action_package_runnable_toolchain(_: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1)


def action_proof_objc3c(_: list[str]) -> int:
    return pwsh_file(PROOF_PS1)




ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-default": action_build_default,
    "build-native-binaries": action_build_native_binaries,
    "build-native-contracts": action_build_native_contracts,
    "build-native-full": action_build_native_full,
    "build-native-reconfigure": action_build_native_reconfigure,
    "build-site": action_build_site,
    "check-site": action_check_site,
    "build-native-docs": action_build_native_docs,
    "check-native-docs": action_check_native_docs,
    "build-public-command-surface": action_build_public_command_surface,
    "check-public-command-surface": action_check_public_command_surface,
    "build-public-command-contract": action_build_public_command_contract,
    "check-public-command-contract": action_check_public_command_contract,
    "check-public-command-budget": action_check_public_command_budget,
    "check-documentation-surface": action_check_documentation_surface,
    "check-markdown": action_check_markdown,
    "format-markdown": action_format_markdown,
    "lint-markdown": action_lint_markdown,
    "lint": action_lint,
    "check-dependency-boundaries": action_check_dependency_boundaries,
    "check-llvm-capabilities": action_check_llvm_capabilities,
    "check-release-evidence": action_check_release_evidence,
    "check-source-hygiene-authenticity": action_check_source_hygiene_authenticity,
    "check-source-hygiene-hard-cutover": action_check_source_hygiene_hard_cutover,
    "check-task-hygiene": action_check_task_hygiene,
    "check-showcase-surface": action_check_showcase_surface,
    "check-stdlib-surface": action_check_stdlib_surface,
    "validate-showcase-runtime": action_validate_showcase_runtime,
    "validate-showcase": action_validate_showcase,
    "validate-runnable-showcase": action_validate_runnable_showcase,
    "validate-getting-started": action_validate_getting_started,
    "check-repo-superclean-surface": action_check_repo_superclean_surface,
    "validate-documentation-surface": action_validate_documentation_surface,
    "validate-repo-superclean": action_validate_repo_superclean,
    "compile-objc3c": action_compile_objc3c,
    "materialize-playground-workspace": action_materialize_playground_workspace,
    "materialize-stdlib-workspace": action_materialize_stdlib_workspace,
    "validate-stdlib-foundation": action_validate_stdlib_foundation,
    "validate-stdlib-advanced": action_validate_stdlib_advanced,
    "validate-stdlib-program": action_validate_stdlib_program,
    "validate-runnable-stdlib-advanced": action_validate_runnable_stdlib_advanced,
    "validate-runnable-stdlib-foundation": action_validate_runnable_stdlib_foundation,
    "validate-runnable-stdlib-program": action_validate_runnable_stdlib_program,
    "inspect-capability-explorer": action_inspect_capability_explorer,
    "inspect-playground-repro": action_inspect_playground_repro,
    "inspect-compile-observability": action_inspect_compile_observability,
    "inspect-runtime-inspector": action_inspect_runtime_inspector,
    "inspect-editor-tooling": action_inspect_editor_tooling,
    "format-objc3c": action_format_objc3c,
    "benchmark-runtime-inspector": action_benchmark_runtime_inspector,
    "benchmark-performance": action_benchmark_performance,
    "benchmark-runtime-performance": action_benchmark_runtime_performance,
    "benchmark-compiler-throughput": action_benchmark_compiler_throughput,
    "validate-compiler-throughput": action_validate_compiler_throughput,
    "validate-runnable-compiler-throughput": action_validate_runnable_compiler_throughput,
    "validate-runtime-performance": action_validate_runtime_performance,
    "validate-runnable-runtime-performance": action_validate_runnable_runtime_performance,
    "benchmark-comparative-baselines": action_benchmark_comparative_baselines,
    "validate-runnable-performance": action_validate_runnable_performance,
    "validate-performance-foundation": action_validate_performance_foundation,
    "validate-conformance-corpus": action_validate_conformance_corpus,
    "validate-runnable-conformance-corpus": action_validate_runnable_conformance_corpus,
    "check-stress-surface": action_check_stress_surface,
    "test-fuzz-safety": action_test_fuzz_safety,
    "test-lowering-runtime-stress": action_test_lowering_runtime_stress,
    "test-mixed-module-differential": action_test_mixed_module_differential,
    "test-stress-minimization": action_test_stress_minimization,
    "test-stress-crash-triage": action_test_stress_crash_triage,
    "validate-stress": action_validate_stress,
    "validate-stress-integration": action_validate_stress_integration,
    "validate-stress-end-to-end": action_validate_stress_end_to_end,
    "check-external-validation-surface": action_check_external_validation_surface,
    "test-external-validation-replay": action_test_external_validation_replay,
    "publish-external-repro-corpus": action_publish_external_repro_corpus,
    "validate-external-validation": action_validate_external_validation,
    "validate-external-validation-integration": action_validate_external_validation_integration,
    "check-public-conformance-reporting-surface": action_check_public_conformance_reporting_surface,
    "check-public-conformance-schema-surface": action_check_public_conformance_schema_surface,
    "build-public-conformance-scorecard": action_build_public_conformance_scorecard,
    "publish-public-conformance-report": action_publish_public_conformance_report,
    "validate-public-conformance-reporting": action_validate_public_conformance_reporting,
    "validate-public-conformance-reporting-integration": action_validate_public_conformance_reporting_integration,
    "validate-public-conformance-reporting-end-to-end": action_validate_public_conformance_reporting_end_to_end,
    "check-performance-governance-surface": action_check_performance_governance_surface,
    "check-performance-governance-schema-surface": action_check_performance_governance_schema_surface,
    "build-performance-dashboard": action_build_performance_dashboard,
    "publish-performance-report": action_publish_performance_report,
    "validate-performance-governance": action_validate_performance_governance,
    "validate-performance-governance-integration": action_validate_performance_governance_integration,
    "validate-performance-governance-end-to-end": action_validate_performance_governance_end_to_end,
    "check-release-foundation-surface": action_check_release_foundation_surface,
    "check-release-foundation-schema-surface": action_check_release_foundation_schema_surface,
    "build-release-manifest": action_build_release_manifest,
    "publish-release-provenance": action_publish_release_provenance,
    "validate-release-foundation": action_validate_release_foundation,
    "check-packaging-channels-surface": action_check_packaging_channels_surface,
    "check-packaging-channels-schema-surface": action_check_packaging_channels_schema_surface,
    "build-package-channels": action_build_package_channels,
    "build-platform-support-matrix": action_build_platform_support_matrix,
    "validate-packaging-channels": action_validate_packaging_channels,
    "validate-packaging-channels-end-to-end": action_validate_packaging_channels_end_to_end,
    "validate-platform-hardening": action_validate_platform_hardening,
    "validate-platform-hardening-end-to-end": action_validate_platform_hardening_end_to_end,
    "check-release-operations-surface": action_check_release_operations_surface,
    "check-release-operations-schema-surface": action_check_release_operations_schema_surface,
    "build-update-manifest": action_build_update_manifest,
    "publish-release-operations": action_publish_release_operations,
    "validate-release-operations": action_validate_release_operations,
    "validate-release-operations-end-to-end": action_validate_release_operations_end_to_end,
    "check-distribution-credibility-surface": action_check_distribution_credibility_surface,
    "check-distribution-credibility-schema-surface": action_check_distribution_credibility_schema_surface,
    "build-distribution-credibility-dashboard": action_build_distribution_credibility_dashboard,
    "publish-distribution-credibility": action_publish_distribution_credibility,
    "validate-distribution-credibility": action_validate_distribution_credibility,
    "validate-distribution-credibility-end-to-end": action_validate_distribution_credibility_end_to_end,
    "check-security-hardening-surface": action_check_security_hardening_surface,
    "check-security-hardening-schema-surface": action_check_security_hardening_schema_surface,
    "build-security-posture": action_build_security_posture,
    "publish-security-advisories": action_publish_security_advisories,
    "validate-security-hardening": action_validate_security_hardening,
    "validate-security-hardening-end-to-end": action_validate_security_hardening_end_to_end,
    "inspect-bonus-tool-integration": action_inspect_bonus_tool_integration,
    "inspect-validation-timing": action_inspect_validation_timing,
    "materialize-project-template": action_materialize_project_template,
    "materialize-canonical-application-workspace": action_materialize_canonical_application_workspace,
    "trace-compile-stages": action_trace_compile_stages,
    "validate-developer-tooling": action_validate_developer_tooling,
    "validate-runnable-developer-tooling": action_validate_runnable_developer_tooling,
    "validate-bonus-experiences": action_validate_bonus_experiences,
    "validate-runnable-bonus-experiences": action_validate_runnable_bonus_experiences,
    "validate-application-architecture": action_validate_application_architecture,
    "validate-runnable-application-architecture": action_validate_runnable_application_architecture,
    "build-package-lock": action_build_package_lock,
    "validate-package-authoring": action_validate_package_authoring,
    "validate-package-mirror": action_validate_package_mirror,
    "validate-package-ecosystem": action_validate_package_ecosystem,
    "validate-runnable-package-ecosystem": action_validate_runnable_package_ecosystem,
    "validate-long-horizon-operations": action_validate_long_horizon_operations,
    "publish-long-horizon-operations": action_publish_long_horizon_operations,
    "validate-adoption-legibility": action_validate_adoption_legibility,
    "publish-adoption-legibility": action_publish_adoption_legibility,
    "validate-governance-sustainability": action_validate_governance_sustainability,
    "publish-governance-sustainability": action_publish_governance_sustainability,
    "publish-planning-issues": action_publish_planning_issues,
    "check-planning-publication-drift": action_check_planning_publication_drift,
    "lint-spec": action_lint_spec,
    "test-default": action_test_default,
    "test-fast": action_test_fast,
    "test-smoke": action_test_smoke,
    "test-ci": action_test_ci,
    "test-recovery": action_test_recovery,
    "test-compile-wrapper-self-audit": action_test_compile_wrapper_self_audit,
    "test-execution-smoke": action_test_execution_smoke,
    "test-execution-replay": action_test_execution_replay,
    "test-execution-replay-focused": action_test_execution_replay_focused,
    "test-runtime-acceptance": action_test_runtime_acceptance,
    "test-runtime-acceptance-fast": action_test_runtime_acceptance_fast,
    "test-runtime-acceptance-diagnostics": action_test_runtime_acceptance_diagnostics,
    "test-runtime-acceptance-cross-module": action_test_runtime_acceptance_cross_module,
    "test-runtime-acceptance-block-arc": action_test_runtime_acceptance_block_arc,
    "test-runtime-acceptance-concurrency": action_test_runtime_acceptance_concurrency,
    "proof-runtime-architecture": action_proof_runtime_architecture,
    "validate-runtime-architecture": action_validate_runtime_architecture,
    "validate-runnable-bootstrap": action_validate_runnable_bootstrap,
    "validate-block-arc-conformance": action_validate_block_arc_conformance,
    "validate-runnable-block-arc": action_validate_runnable_block_arc,
    "validate-concurrency-conformance": action_validate_concurrency_conformance,
    "validate-runnable-concurrency": action_validate_runnable_concurrency,
    "validate-object-model-conformance": action_validate_object_model_conformance,
    "validate-storage-reflection-conformance": action_validate_storage_reflection_conformance,
    "validate-runnable-object-model": action_validate_runnable_object_model,
    "validate-runnable-storage-reflection": action_validate_runnable_storage_reflection,
    "validate-error-conformance": action_validate_error_conformance,
    "validate-runnable-error": action_validate_runnable_error,
    "validate-interop-conformance": action_validate_interop_conformance,
    "validate-runnable-interop": action_validate_runnable_interop,
    "validate-metaprogramming-conformance": action_validate_metaprogramming_conformance,
    "validate-runnable-metaprogramming": action_validate_runnable_metaprogramming,
    "validate-release-candidate-conformance": action_validate_release_candidate_conformance,
    "validate-runnable-release-candidate": action_validate_runnable_release_candidate,
    "test-fixture-matrix": action_test_fixture_matrix,
    "test-negative-expectations": action_test_negative_expectations,
    "test-full": action_test_full,
    "test-nightly": action_test_nightly,
    "package-runnable-toolchain": action_package_runnable_toolchain,
    "proof-objc3c": action_proof_objc3c,
}



def enrich_action_payload(spec: ActionSpec) -> dict[str, object]:
    payload = asdict(spec)
    payload["mode"] = WORKFLOW_RUNNER_MODE
    payload["runner_path"] = WORKFLOW_RUNNER_SURFACE
    payload["category"] = spec.action.split("-", 1)[0]
    return payload


def list_actions_payload() -> dict[str, object]:
    return {
        "mode": WORKFLOW_RUNNER_MODE,
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "action_count": len(ACTION_SPECS),
        "public_action_count": len(ACTION_SPECS),
        "internal_action_count": 0,
        "public_script_count": 1,
        "actions": [enrich_action_payload(spec) for spec in ACTION_SPECS.values()],
    }


def describe_action_payload(action: str) -> dict[str, object]:
    return enrich_action_payload(ACTION_SPECS[action])



def execute_registered_action(action: str, rest: list[str]) -> int:
    spec = ACTION_SPECS.get(action)
    handler = ACTION_HANDLERS.get(action)
    if spec is None or handler is None:
        print(f"unknown action: {action}", file=sys.stderr)
        return 2
    if rest and not spec.pass_through_args:
        print(f"action does not accept extra arguments: {action}", file=sys.stderr)
        return 2
    return handler(rest)


def main(argv: Sequence[str]) -> int:
    if not argv:
        print(
            f"usage: {WORKFLOW_COMMAND_TEXT} <action> [args...]\n"
            f"       {WORKFLOW_COMMAND_TEXT} --list-json\n"
            f"       {WORKFLOW_COMMAND_TEXT} --describe <action>\n"
            f"       {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>",
            file=sys.stderr,
        )
        return 2

    action, *rest = argv
    if action == "--list-json":
        return emit_json(list_actions_payload())
    if action == "--describe":
        if len(rest) != 1:
            print(f"usage: {WORKFLOW_COMMAND_TEXT} --describe <action>", file=sys.stderr)
            return 2
        describe_action = rest[0]
        if describe_action not in ACTION_SPECS:
            print(f"unknown action: {describe_action}", file=sys.stderr)
            return 2
        return emit_json(describe_action_payload(describe_action))
    if action == "--describe-script":
        if len(rest) != 1:
            print(f"usage: {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>", file=sys.stderr)
            return 2
        describe_script = rest[0]
        if describe_script != "objc3c":
            print(f"unknown package script: {describe_script}", file=sys.stderr)
            return 2
        return emit_json(describe_package_script_payload(describe_script))
    return execute_registered_action(action, rest)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
