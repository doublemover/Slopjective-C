"""Runtime acceptance summary assembly."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.diagnostics import build_probe_retry_policy
from objc3c_runtime_acceptance.dispatch import (
    build_dispatch_accessor_runtime_abi_surface,
    build_property_ivar_accessor_reflection_implementation_surface,
    build_storage_accessor_runtime_abi_surface,
)
from objc3c_runtime_acceptance.domains.probe_helpers import (
    build_runtime_bootstrap_lowering_registration_artifact_surface,
    build_runtime_bootstrap_registration_source_surface,
    build_runtime_state_publication_surface,
)
from objc3c_runtime_acceptance.native_build import (
    ACCEPTANCE_ARTIFACT_REGISTRY,
    COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
    COMPILE_PROVENANCE_CONTRACT_ID,
    DEFAULT_COMPILE_BACKEND,
    DIRECT_COMPILE_BACKEND,
    RUNTIME_LIB,
    ROOT,
    WRAPPER_COMPILE_BACKEND,
)
from objc3c_runtime_acceptance.probes import ACCEPTANCE_PROBE_RETRY_EVENTS
from objc3c_runtime_acceptance.progress import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.progress import repo_display_path
from objc3c_runtime_acceptance.runtime_contracts import (
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.surfaces import (
    build_acceptance_suite_surface,
    build_claim_boundary,
)


def build_runtime_acceptance_summary(
    *,
    args: Any,
    run_dir: Path,
    report_path: Path,
    progress_path: Path,
    clangxx: str,
    results: list[CaseResult],
    acceptance_progress: RuntimeAcceptanceProgress,
    domains: RuntimeAcceptanceDomains,
    available_suites: dict[str, tuple[str, ...]],
) -> dict[str, Any]:
    return {
        "status": "PASS",
        "run_dir": str(run_dir.relative_to(ROOT)).replace("\\", "/"),
        "clangxx": clangxx,
        "runtime_library": str(RUNTIME_LIB.relative_to(ROOT)).replace("\\", "/"),
        "case_count": len(results),
        "selected_suite": args.suite,
        "selected_cases": list(args.cases),
        "available_suites": {
            suite: list(cases) if cases else "all"
            for suite, cases in available_suites.items()
        },
        "default_compile_backend": DEFAULT_COMPILE_BACKEND,
        "direct_compile_backend": DIRECT_COMPILE_BACKEND,
        "wrapper_compile_backend": WRAPPER_COMPILE_BACKEND,
        "probe_retry_policy": build_probe_retry_policy(),
        "probe_retry_events": ACCEPTANCE_PROBE_RETRY_EVENTS,
        "progress_report_path": repo_display_path(progress_path),
        "timing": acceptance_progress.final_summary(),
        "artifact_registry": ACCEPTANCE_ARTIFACT_REGISTRY.summary(),
        "cases": [
            {
                "case_id": result.case_id,
                "probe": result.probe,
                "fixture": result.fixture,
                "claim_class": result.claim_class,
                "passed": result.passed,
                "summary": result.summary,
            }
            for result in results
        ],
        "claim_boundary": build_claim_boundary(PUBLIC_RUNTIME_ABI_BOUNDARY),
        "runtime_state_publication_surface": build_runtime_state_publication_surface(
            PUBLIC_RUNTIME_ABI_BOUNDARY
        ),
        "runtime_bootstrap_registration_source_surface": build_runtime_bootstrap_registration_source_surface(),
        "runtime_bootstrap_lowering_registration_artifact_surface": (
            build_runtime_bootstrap_lowering_registration_artifact_surface()
        ),
        "runtime_multi_image_startup_ordering_source_surface": (
            domains.registration.build_runtime_multi_image_startup_ordering_source_surface(
                results
            )
        ),
        "runtime_metaprogramming_source_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_source_surface(results)
        ),
        "runtime_metaprogramming_package_provenance_source_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_package_provenance_source_surface(
                results
            )
        ),
        "runtime_metaprogramming_semantics_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_semantics_surface(
                results
            )
        ),
        "runtime_metaprogramming_lowering_host_cache_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_lowering_host_cache_surface(
                results
            )
        ),
        "runtime_cross_module_metaprogramming_artifact_preservation_surface": (
            domains.metaprogramming.build_runtime_cross_module_metaprogramming_artifact_preservation_surface(
                results
            )
        ),
        "runtime_metaprogramming_runtime_abi_cache_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_runtime_abi_cache_surface(
                results
            )
        ),
        "runtime_metaprogramming_cache_runtime_integration_implementation_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_cache_runtime_integration_implementation_surface(
                results
            )
        ),
        "runtime_cross_module_package_interop_source_surface": (
            domains.interop_packaging.build_runtime_cross_module_package_interop_source_surface(
                results
            )
        ),
        "runtime_textual_binary_interface_parity_source_surface": (
            domains.interop_packaging.build_runtime_textual_binary_interface_parity_source_surface(
                results
            )
        ),
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
            domains.release_claims.build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface(
                results
            )
        ),
        "runtime_strict_profile_feature_claim_source_surface": (
            domains.release_claims.build_runtime_strict_profile_feature_claim_source_surface(
                results
            )
        ),
        "runtime_claimability_semantics_release_policy_surface": (
            domains.release_claims.build_runtime_claimability_semantics_release_policy_surface(
                results
            )
        ),
        "runtime_strict_profile_claim_implementation_surface": (
            domains.release_claims.build_runtime_strict_profile_claim_implementation_surface(
                results
            )
        ),
        "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface": (
            domains.release_claims.build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface(
                results
            )
        ),
        "runtime_claim_publication_dashboard_schema_surface": (
            domains.release_claims.build_runtime_claim_publication_dashboard_schema_surface(
                results
            )
        ),
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
            domains.release_claims.build_runtime_final_claim_publication_deprecated_path_shutdown_surface(
                results
            )
        ),
        "runtime_release_candidate_claim_abi_surface": (
            domains.release_claims.build_runtime_release_candidate_claim_abi_surface(
                results
            )
        ),
        "runtime_final_release_evidence_descaffolding_implementation_surface": (
            domains.release_claims.build_runtime_final_release_evidence_descaffolding_implementation_surface(
                results
            )
        ),
        "runtime_mixed_image_compatibility_interop_semantics_surface": (
            domains.interop_packaging.build_runtime_mixed_image_compatibility_interop_semantics_surface(
                results
            )
        ),
        "runtime_package_loading_module_identity_semantics_surface": (
            domains.interop_packaging.build_runtime_package_loading_module_identity_semantics_surface(
                results
            )
        ),
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": (
            domains.interop_packaging.build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface(
                results
            )
        ),
        "runtime_import_version_feature_claim_diagnostics_surface": (
            domains.interop_packaging.build_runtime_import_version_feature_claim_diagnostics_surface(
                results
            )
        ),
        "runtime_packaging_bridge_loader_artifact_surface": (
            domains.interop_packaging.build_runtime_packaging_bridge_loader_artifact_surface(
                results
            )
        ),
        "runtime_mixed_image_package_lowering_bridge_emission_surface": (
            domains.interop_packaging.build_runtime_mixed_image_package_lowering_bridge_emission_surface(
                results
            )
        ),
        "runtime_cross_language_replay_import_surface_preservation_surface": (
            domains.interop_packaging.build_runtime_cross_language_replay_import_surface_preservation_surface(
                results
            )
        ),
        "runtime_package_loader_bridge_abi_surface": (
            domains.interop_packaging.build_runtime_package_loader_bridge_abi_surface(
                results
            )
        ),
        "runtime_package_loading_interop_implementation_surface": (
            domains.interop_packaging.build_runtime_package_loading_interop_implementation_surface(
                results
            )
        ),
        "runtime_unified_concurrency_source_surface": (
            domains.concurrency.build_runtime_unified_concurrency_source_surface(
                results
            )
        ),
        "runtime_async_task_actor_normalization_completion_surface": (
            domains.concurrency.build_runtime_async_task_actor_normalization_completion_surface(
                results
            )
        ),
        "runtime_unified_concurrency_lowering_metadata_surface": (
            domains.concurrency.build_runtime_unified_concurrency_lowering_metadata_surface(
                results
            )
        ),
        "runtime_unified_concurrency_runtime_abi_surface": (
            domains.concurrency.build_runtime_unified_concurrency_runtime_abi_surface(
                results
            )
        ),
        "runtime_error_execution_cleanup_source_surface": (
            domains.errors.build_runtime_error_execution_cleanup_source_surface(results)
        ),
        "runtime_catch_filter_finalization_source_surface": (
            domains.errors.build_runtime_catch_filter_finalization_source_surface(
                results
            )
        ),
        "runtime_error_propagation_cleanup_semantics_surface": (
            domains.errors.build_runtime_error_propagation_cleanup_semantics_surface(
                results
            )
        ),
        "runtime_bridging_filter_unwind_diagnostics_surface": (
            domains.errors.build_runtime_bridging_filter_unwind_diagnostics_surface(
                results
            )
        ),
        "runtime_error_lowering_unwind_bridge_helper_surface": (
            domains.errors.build_runtime_error_lowering_unwind_bridge_helper_surface(
                results
            )
        ),
        "runtime_error_runtime_abi_cleanup_surface": (
            domains.errors.build_runtime_error_runtime_abi_cleanup_surface(results)
        ),
        "runtime_error_propagation_catch_cleanup_runtime_implementation_surface": (
            domains.errors.build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface(
                results
            )
        ),
        "runtime_object_model_realization_source_surface": (
            domains.object_model.build_runtime_object_model_realization_source_surface(
                results
            )
        ),
        "runtime_block_arc_unified_source_surface": (
            domains.block_arc.build_runtime_block_arc_unified_source_surface(results)
        ),
        "runtime_ownership_transfer_capture_family_source_surface": (
            domains.block_arc.build_runtime_ownership_transfer_capture_family_source_surface(
                results
            )
        ),
        "runtime_block_arc_lowering_helper_surface": (
            domains.block_arc.build_runtime_block_arc_lowering_helper_surface(results)
        ),
        "runtime_block_arc_runtime_abi_surface": (
            domains.block_arc.build_runtime_block_arc_runtime_abi_surface(results)
        ),
        "runtime_property_ivar_storage_accessor_source_surface": (
            domains.storage_reflection.build_runtime_property_ivar_storage_accessor_source_surface(
                results
            )
        ),
        "runtime_property_atomicity_synthesis_reflection_source_surface": (
            domains.storage_reflection.build_runtime_property_atomicity_synthesis_reflection_source_surface(
                results
            )
        ),
        "dispatch_and_synthesized_accessor_lowering_surface": (
            domains.storage_reflection.build_dispatch_and_synthesized_accessor_lowering_surface(
                results
            )
        ),
        "executable_property_accessor_layout_lowering_surface": (
            domains.storage_reflection.build_executable_property_accessor_layout_lowering_surface(
                results
            )
        ),
        "executable_ivar_layout_emission_surface": (
            domains.storage_reflection.build_executable_ivar_layout_emission_surface(
                results
            )
        ),
        "executable_synthesized_accessor_property_lowering_surface": (
            domains.storage_reflection.build_executable_synthesized_accessor_property_lowering_surface(
                results
            )
        ),
        "runtime_realization_lowering_reflection_artifact_surface": (
            domains.object_model.build_runtime_realization_lowering_reflection_artifact_surface(
                results
            )
        ),
        "runtime_dispatch_table_reflection_record_lowering_surface": (
            domains.object_model.build_runtime_dispatch_table_reflection_record_lowering_surface(
                results
            )
        ),
        "runtime_cross_module_realized_metadata_replay_preservation_surface": (
            domains.object_model.build_runtime_cross_module_realized_metadata_replay_preservation_surface(
                results
            )
        ),
        "runtime_object_model_abi_query_surface": (
            domains.object_model.build_runtime_object_model_abi_query_surface(results)
        ),
        "runtime_realization_lookup_reflection_implementation_surface": (
            domains.object_model.build_runtime_realization_lookup_reflection_implementation_surface(
                results
            )
        ),
        "runtime_reflection_query_surface": (
            domains.object_model.build_runtime_reflection_query_surface(results)
        ),
        "runtime_realization_lookup_semantics_surface": (
            domains.object_model.build_runtime_realization_lookup_semantics_surface(
                results
            )
        ),
        "runtime_class_metaclass_protocol_realization_surface": (
            domains.object_model.build_runtime_class_metaclass_protocol_realization_surface(
                results
            )
        ),
        "runtime_category_attachment_merged_dispatch_surface": (
            domains.object_model.build_runtime_category_attachment_merged_dispatch_surface(
                results
            )
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface": (
            domains.object_model.build_runtime_reflection_visibility_coherence_diagnostics_surface(
                results
            )
        ),
        "acceptance_suite_surface": build_acceptance_suite_surface(
            results,
            report_path,
            root=ROOT,
            runtime_acceptance_suite_surface_contract_id=(
                RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID
            ),
            runtime_state_publication_surface_contract_id=(
                RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID
            ),
            compile_provenance_contract_id=COMPILE_PROVENANCE_CONTRACT_ID,
            compile_output_truthfulness_contract_id=(
                COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
            ),
        ),
        "runtime_installation_abi_surface": (
            domains.registration.build_runtime_installation_abi_surface()
        ),
        "runtime_loader_lifecycle_surface": (
            domains.registration.build_runtime_loader_lifecycle_surface(results)
        ),
        "dispatch_accessor_runtime_abi_surface": (
            build_dispatch_accessor_runtime_abi_surface()
        ),
        "storage_accessor_runtime_abi_surface": (
            build_storage_accessor_runtime_abi_surface()
        ),
        "runtime_property_ivar_accessor_reflection_implementation_surface": (
            build_property_ivar_accessor_reflection_implementation_surface()
        ),
    }


__all__ = ["build_runtime_acceptance_summary"]
