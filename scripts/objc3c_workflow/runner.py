#!/usr/bin/env python3
"""Unified public workflow runner for the live objc3c command surface."""

from __future__ import annotations

import json
import os
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

from objc3c_tooling.subprocesses import run_capture
from objc3c_tooling.public_workflow_output import extract_public_workflow_report_paths as extract_report_paths

from scripts.objc3c_workflow.action_spec import ActionHandler, ActionSpec
from scripts.objc3c_workflow.actions.application_surfaces import (
    CONFORMANCE_CORPUS_INTEGRATION_PY,
    STDLIB_ADVANCED_INTEGRATION_PY,
    STDLIB_FOUNDATION_INTEGRATION_PY,
    STDLIB_PROGRAM_INTEGRATION_PY,
    action_check_showcase_surface,
    action_check_stdlib_surface,
    action_materialize_canonical_application_workspace,
    action_materialize_stdlib_workspace,
    action_validate_application_architecture,
    action_validate_conformance_corpus,
    action_validate_getting_started,
    action_validate_runnable_application_architecture,
    action_validate_runnable_conformance_corpus,
    action_validate_runnable_showcase,
    action_validate_runnable_stdlib_advanced,
    action_validate_runnable_stdlib_foundation,
    action_validate_runnable_stdlib_program,
    action_validate_showcase,
    action_validate_showcase_runtime,
    action_validate_stdlib_advanced,
    action_validate_stdlib_foundation,
    action_validate_stdlib_program,
)
from scripts.objc3c_workflow.actions.developer_tooling import (
    BONUS_EXPERIENCE_INTEGRATION_PY,
    DEVELOPER_TOOLING_INTEGRATION_PY,
    action_check_llvm_capabilities,
    action_format_objc3c,
    action_inspect_bonus_tool_integration,
    action_inspect_capability_explorer,
    action_inspect_compile_observability,
    action_inspect_editor_tooling,
    action_inspect_playground_repro,
    action_inspect_runtime_inspector,
    action_materialize_playground_workspace,
    action_materialize_project_template,
    action_trace_compile_stages,
    action_validate_bonus_experiences,
    action_validate_developer_tooling,
    action_validate_runnable_bonus_experiences,
    action_validate_runnable_developer_tooling,
)
from scripts.objc3c_workflow.actions.docs import (
    action_build_native_docs,
    action_build_public_command_contract,
    action_build_public_command_surface,
    action_build_site,
    action_check_documentation_surface,
    action_check_markdown,
    action_check_native_docs,
    action_check_public_command_budget,
    action_check_public_command_contract,
    action_check_public_command_surface,
    action_check_site,
    action_format_markdown,
    action_lint_markdown,
    action_validate_documentation_surface,
)
from scripts.objc3c_workflow.actions.ecosystem_publication import (
    action_build_package_lock,
    action_check_planning_publication_drift,
    action_publish_adoption_legibility,
    action_publish_governance_sustainability,
    action_publish_long_horizon_operations,
    action_publish_planning_issues,
    action_validate_adoption_legibility,
    action_validate_governance_sustainability,
    action_validate_long_horizon_operations,
    action_validate_package_authoring,
    action_validate_package_ecosystem,
    action_validate_package_mirror,
    action_validate_runnable_package_ecosystem,
)
from scripts.objc3c_workflow.actions.external_validation import (
    action_check_external_validation_surface,
    action_publish_external_repro_corpus,
    action_test_external_validation_replay,
    action_validate_external_validation,
    action_validate_external_validation_integration,
)
from scripts.objc3c_workflow.actions.native_build import (
    BUILD_PS1,
    action_build_native_binaries,
    action_build_native_contracts,
    action_build_native_full,
    action_build_native_reconfigure,
    action_compile_objc3c,
    action_package_runnable_toolchain,
    action_proof_objc3c,
)
from scripts.objc3c_workflow.actions.performance import (
    action_benchmark_comparative_baselines,
    action_benchmark_compiler_throughput,
    action_benchmark_performance,
    action_benchmark_runtime_inspector,
    action_benchmark_runtime_performance,
    action_build_performance_dashboard,
    action_check_performance_governance_schema_surface,
    action_check_performance_governance_surface,
    action_publish_performance_report,
    action_validate_compiler_throughput,
    action_validate_performance_foundation,
    action_validate_performance_governance,
    action_validate_performance_governance_end_to_end,
    action_validate_performance_governance_integration,
    action_validate_runnable_compiler_throughput,
    action_validate_runnable_performance,
    action_validate_runnable_runtime_performance,
    action_validate_runtime_performance,
)
from scripts.objc3c_workflow.actions.runtime_tests import (
    action_proof_runtime_architecture,
    action_test_runtime_acceptance,
    action_test_runtime_acceptance_block_arc,
    action_test_runtime_acceptance_concurrency,
    action_test_runtime_acceptance_cross_module,
    action_test_runtime_acceptance_diagnostics,
    action_test_runtime_acceptance_fast,
    action_validate_block_arc_conformance,
    action_validate_concurrency_conformance,
    action_validate_error_conformance,
    action_validate_interop_conformance,
    action_validate_metaprogramming_conformance,
    action_validate_object_model_conformance,
    action_validate_release_candidate_conformance,
    action_validate_runnable_block_arc,
    action_validate_runnable_bootstrap,
    action_validate_runnable_concurrency,
    action_validate_runnable_error,
    action_validate_runnable_interop,
    action_validate_runnable_metaprogramming,
    action_validate_runnable_object_model,
    action_validate_runnable_release_candidate,
    action_validate_runnable_storage_reflection,
    action_validate_runtime_architecture,
    action_validate_storage_reflection_conformance,
)
from scripts.objc3c_workflow.actions.release_governance import (
    action_build_distribution_credibility_dashboard,
    action_build_package_channels,
    action_build_platform_support_matrix,
    action_build_public_conformance_scorecard,
    action_build_release_manifest,
    action_build_security_posture,
    action_build_update_manifest,
    action_check_distribution_credibility_surface,
    action_check_packaging_channels_surface,
    action_check_public_conformance_reporting_surface,
    action_check_release_foundation_surface,
    action_check_release_operations_surface,
    action_check_security_hardening_surface,
    action_publish_distribution_credibility,
    action_publish_public_conformance_report,
    action_publish_release_operations,
    action_publish_release_provenance,
    action_publish_security_advisories,
    action_validate_distribution_credibility,
    action_validate_distribution_credibility_end_to_end,
    action_validate_packaging_channels,
    action_validate_packaging_channels_end_to_end,
    action_validate_platform_hardening,
    action_validate_platform_hardening_end_to_end,
    action_validate_public_conformance_reporting,
    action_validate_public_conformance_reporting_end_to_end,
    action_validate_public_conformance_reporting_integration,
    action_validate_release_foundation,
    action_validate_release_operations,
    action_validate_release_operations_end_to_end,
    action_validate_security_hardening,
    action_validate_security_hardening_end_to_end,
)
from scripts.objc3c_workflow.actions.schema_surfaces import (
    action_check_distribution_credibility_schema_surface,
    action_check_packaging_channels_schema_surface,
    action_check_public_conformance_schema_surface,
    action_check_release_foundation_schema_surface,
    action_check_release_operations_schema_surface,
    action_check_security_hardening_schema_surface,
)
from scripts.objc3c_workflow.actions.stress import (
    action_check_stress_surface,
    action_test_fuzz_safety,
    action_test_lowering_runtime_stress,
    action_test_mixed_module_differential,
    action_test_stress_crash_triage,
    action_test_stress_minimization,
    action_validate_stress,
    action_validate_stress_end_to_end,
    action_validate_stress_integration,
)
from scripts.objc3c_workflow.actions.validation_timing import (
    action_inspect_validation_timing,
    collect_child_timing,
    load_latest_report_payload,
    load_surface_from_report,
    validation_budget_violations,
    validation_speed_budget_mode,
)
from scripts.objc3c_workflow.commands import pwsh_file, run, workflow_command
from scripts.objc3c_workflow.environment import (
    PWSH,
    WORKFLOW_COMMAND_TEXT,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from scripts.objc3c_workflow.npm_surface import describe_package_script_payload
from scripts.objc3c_workflow.registry import ACTION_SPECS
from scripts.objc3c_workflow.reports import emit_json

COMPILE_WRAPPER_SELF_AUDIT_PY = (
    ROOT / "scripts" / "check_objc3c_compile_wrapper_self_audit.py"
)
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
MATRIX_PS1 = ROOT / "scripts" / "run_objc3c_native_fixture_matrix.ps1"
NEGATIVE_EXPECTATIONS_PS1 = ROOT / "scripts" / "check_objc3c_negative_fixture_expectations.ps1"
REPO_SUPERCLEAN_SURFACE_PY = ROOT / "scripts" / "check_repo_superclean_surface.py"
DEPENDENCY_BOUNDARIES_PY = ROOT / "scripts" / "check_objc3c_dependency_boundaries.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
SOURCE_HYGIENE_AUTHENTICITY_PY = ROOT / "scripts" / "check_source_hygiene_authenticity.py"
SOURCE_HYGIENE_HARD_CUTOVER_PY = ROOT / "scripts" / "check_source_hygiene_hard_cutover.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
BEHAVIOR_MATRIX_PY = ROOT / "scripts" / "check_objc3c_behavior_matrix.py"
RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
def run_steps(actions: Sequence[str]) -> int:
    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


def action_build_default(_: list[str]) -> int:
    return run_steps(["build-native-binaries"])


def action_check_dependency_boundaries(_: list[str]) -> int:
    return run([sys.executable, str(DEPENDENCY_BOUNDARIES_PY), "--strict"])


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


def action_validate_repo_superclean(_: list[str]) -> int:
    return run_composite_validation(
        "validate-repo-superclean",
        [
            ("build-native-contracts", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(BUILD_PS1), "-ExecutionMode", "contracts-binary"]),
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("source-hygiene", [sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)]),
        ],
    )


def action_lint_spec(_: list[str]) -> int:
    return run([sys.executable, str(SPEC_LINT_PY)])


def action_lint(_: list[str]) -> int:
    return run_steps(["check-source-hygiene-hard-cutover", "check-task-hygiene", "build-site", "check-markdown"])


def action_test_default(_: list[str]) -> int:
    return run_steps(["test-smoke"])


def to_repo_relative(raw_path: str) -> str:
    candidate = Path(raw_path.strip())
    try:
        if candidate.is_absolute():
            return str(candidate.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return raw_path.strip()
    return raw_path.strip().replace("\\", "/")

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


def action_test_behavior_matrix(_: list[str]) -> int:
    return run([sys.executable, str(BEHAVIOR_MATRIX_PY)])


def action_test_smoke(_: list[str]) -> int:
    return run_composite_validation(
        "test-smoke",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
            ("test-runtime-acceptance-fast", [sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"]),
            ("test-execution-replay-focused", [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(REPLAY_PS1), "-Limit", "1"]),
        ],
    )


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


def action_test_compile_wrapper_self_audit(_: list[str]) -> int:
    return run([sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)


def action_test_full(_: list[str]) -> int:
    return run_composite_validation(
        "test-full",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
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
    "test-behavior-matrix": action_test_behavior_matrix,
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
