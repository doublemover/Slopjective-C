"""Composite public workflow execution and report emission."""

from __future__ import annotations

import json
import os
import sys
from collections.abc import Sequence
from datetime import datetime, timezone
from pathlib import Path
from time import perf_counter

from objc3c_tooling.public_workflow_output import (
    extract_public_workflow_report_paths as extract_report_paths,
)
from objc3c_tooling.subprocesses import run_capture

from .actions.validation_timing import (
    collect_child_timing,
    load_latest_report_payload,
    load_surface_from_report,
    validation_budget_violations,
    validation_speed_budget_mode,
)
from .environment import ROOT, WORKFLOW_RUNNER_SURFACE

PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
RUNNER_SCRIPT_PATH = Path(__file__).with_name("runner.py").resolve()

COMPOSITE_SURFACE_KEYS = (
    "runtime_state_publication_surface",
    "runtime_error_execution_cleanup_source_surface",
    "runtime_catch_filter_finalization_source_surface",
    "runtime_error_propagation_cleanup_semantics_surface",
    "runtime_bridging_filter_unwind_diagnostics_surface",
    "runtime_error_lowering_unwind_bridge_helper_surface",
    "runtime_error_runtime_abi_cleanup_surface",
    "runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
    "acceptance_suite_surface",
    "runtime_installation_abi_surface",
    "runtime_loader_lifecycle_surface",
    "runtime_object_model_realization_source_surface",
    "runtime_block_arc_unified_source_surface",
    "runtime_ownership_transfer_capture_family_source_surface",
    "runtime_block_arc_lowering_helper_surface",
    "runtime_block_arc_runtime_abi_surface",
    "runtime_property_ivar_storage_accessor_source_surface",
    "storage_accessor_runtime_abi_surface",
    "runtime_property_ivar_accessor_reflection_implementation_surface",
    "runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
    "runtime_strict_profile_feature_claim_source_surface",
    "runtime_claimability_semantics_release_policy_surface",
    "runtime_strict_profile_claim_implementation_surface",
    "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface",
    "runtime_claim_publication_dashboard_schema_surface",
    "runtime_final_claim_publication_deprecated_path_shutdown_surface",
    "runtime_release_candidate_claim_abi_surface",
    "runtime_final_release_evidence_descaffolding_implementation_surface",
    "runtime_metaprogramming_source_surface",
    "runtime_metaprogramming_package_provenance_source_surface",
    "runtime_metaprogramming_semantics_surface",
    "runtime_metaprogramming_lowering_host_cache_surface",
    "runtime_cross_module_metaprogramming_artifact_preservation_surface",
    "runtime_metaprogramming_runtime_abi_cache_surface",
    "runtime_metaprogramming_cache_runtime_integration_implementation_surface",
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
    "runtime_property_atomicity_synthesis_reflection_source_surface",
    "runtime_realization_lowering_reflection_artifact_surface",
    "runtime_dispatch_table_reflection_record_lowering_surface",
    "runtime_cross_module_realized_metadata_replay_preservation_surface",
    "runtime_object_model_abi_query_surface",
    "runtime_realization_lookup_reflection_implementation_surface",
    "runtime_reflection_query_surface",
    "runtime_realization_lookup_semantics_surface",
    "runtime_class_metaclass_protocol_realization_surface",
    "runtime_category_attachment_merged_dispatch_surface",
    "runtime_reflection_visibility_coherence_diagnostics_surface",
)


def attach_child_surfaces(
    payload: dict[str, object],
    steps: Sequence[dict[str, object]],
) -> None:
    for surface_key in COMPOSITE_SURFACE_KEYS:
        surface = load_surface_from_report(steps, surface_key)
        if surface is not None:
            payload[surface_key] = surface


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
        validation_budget_violations(budgets) if isinstance(budgets, list) else []
    )
    effective_status = "FAIL" if status == "PASS" and budget_violations else status
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
    attach_child_surfaces(payload, steps)
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return report_path


def execute_nested_action(action: str, rest: list[str]) -> int:
    from .action_dispatch import execute_registered_action

    return execute_registered_action(action, rest)


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
        and Path(normalized[1]).resolve() == RUNNER_SCRIPT_PATH
    ):
        nested_action = normalized[2]
        nested_rest = normalized[3:]
        exit_code = execute_nested_action(nested_action, nested_rest)
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
            report_path = write_composite_validation_report(
                action, results, status="FAIL"
            )
            print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
            return int(step["exit_code"])
    report_path = write_composite_validation_report(action, results, status="PASS")
    print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
    report_payload = load_latest_report_payload(report_path)
    if isinstance(report_payload, dict) and report_payload.get("status") != "PASS":
        return 1
    return 0
