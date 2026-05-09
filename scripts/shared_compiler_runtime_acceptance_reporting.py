"""Report projection for the shared executable runtime acceptance harness."""

from __future__ import annotations

from typing import Any

from shared_compiler_runtime_acceptance_catalog import SuiteEntry


def summarize_report(entry: SuiteEntry, report: dict[str, Any], surfaces: dict[str, dict[str, Any]]) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "suite_id": entry.suite_id,
        "summary": entry.summary,
        "execution_kind": entry.execution_kind,
        "validation_owner": entry.validation_owner,
        "guarantee_owner": entry.guarantee_owner,
        "report_path": entry.report_path,
        "claim_boundary": surfaces["claim_boundary"],
        "runtime_state_publication_surface": surfaces["runtime_state_publication_surface"],
        "acceptance_suite_surface": surfaces["acceptance_suite_surface"],
        "runtime_error_execution_cleanup_source_surface": surfaces[
            "runtime_error_execution_cleanup_source_surface"
        ],
        "runtime_catch_filter_finalization_source_surface": surfaces[
            "runtime_catch_filter_finalization_source_surface"
        ],
        "runtime_error_propagation_cleanup_semantics_surface": surfaces[
            "runtime_error_propagation_cleanup_semantics_surface"
        ],
        "runtime_bridging_filter_unwind_diagnostics_surface": surfaces[
            "runtime_bridging_filter_unwind_diagnostics_surface"
        ],
        "runtime_error_lowering_unwind_bridge_helper_surface": surfaces[
            "runtime_error_lowering_unwind_bridge_helper_surface"
        ],
        "runtime_error_runtime_abi_cleanup_surface": surfaces[
            "runtime_error_runtime_abi_cleanup_surface"
        ],
        "runtime_error_propagation_catch_cleanup_runtime_implementation_surface": surfaces[
            "runtime_error_propagation_catch_cleanup_runtime_implementation_surface"
        ],
        "runtime_object_model_realization_source_surface": surfaces[
            "runtime_object_model_realization_source_surface"
        ],
        "runtime_block_arc_unified_source_surface": surfaces[
            "runtime_block_arc_unified_source_surface"
        ],
        "runtime_ownership_transfer_capture_family_source_surface": surfaces[
            "runtime_ownership_transfer_capture_family_source_surface"
        ],
        "runtime_block_arc_lowering_helper_surface": surfaces[
            "runtime_block_arc_lowering_helper_surface"
        ],
        "runtime_block_arc_runtime_abi_surface": surfaces[
            "runtime_block_arc_runtime_abi_surface"
        ],
        "runtime_property_ivar_storage_accessor_source_surface": surfaces[
            "runtime_property_ivar_storage_accessor_source_surface"
        ],
        "storage_accessor_runtime_abi_surface": surfaces[
            "storage_accessor_runtime_abi_surface"
        ],
        "runtime_property_ivar_accessor_reflection_implementation_surface": surfaces[
            "runtime_property_ivar_accessor_reflection_implementation_surface"
        ],
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": surfaces[
            "runtime_claimable_surface_residual_non_claimable_gaps_source_surface"
        ],
        "runtime_strict_profile_feature_claim_source_surface": surfaces[
            "runtime_strict_profile_feature_claim_source_surface"
        ],
        "runtime_claimability_semantics_release_policy_surface": surfaces[
            "runtime_claimability_semantics_release_policy_surface"
        ],
        "runtime_strict_profile_claim_implementation_surface": surfaces[
            "runtime_strict_profile_claim_implementation_surface"
        ],
        "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface": surfaces[
            "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface"
        ],
        "runtime_claim_publication_dashboard_schema_surface": surfaces[
            "runtime_claim_publication_dashboard_schema_surface"
        ],
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": surfaces[
            "runtime_final_claim_publication_deprecated_path_shutdown_surface"
        ],
        "runtime_release_candidate_claim_abi_surface": surfaces[
            "runtime_release_candidate_claim_abi_surface"
        ],
        "runtime_final_release_evidence_descaffolding_implementation_surface": surfaces[
            "runtime_final_release_evidence_descaffolding_implementation_surface"
        ],
        "runtime_metaprogramming_source_surface": surfaces[
            "runtime_metaprogramming_source_surface"
        ],
        "runtime_metaprogramming_package_provenance_source_surface": surfaces[
            "runtime_metaprogramming_package_provenance_source_surface"
        ],
        "runtime_metaprogramming_semantics_surface": surfaces[
            "runtime_metaprogramming_semantics_surface"
        ],
        "runtime_metaprogramming_lowering_host_cache_surface": surfaces[
            "runtime_metaprogramming_lowering_host_cache_surface"
        ],
        "runtime_cross_module_metaprogramming_artifact_preservation_surface": surfaces[
            "runtime_cross_module_metaprogramming_artifact_preservation_surface"
        ],
        "runtime_metaprogramming_runtime_abi_cache_surface": surfaces[
            "runtime_metaprogramming_runtime_abi_cache_surface"
        ],
        "runtime_metaprogramming_cache_runtime_integration_implementation_surface": surfaces[
            "runtime_metaprogramming_cache_runtime_integration_implementation_surface"
        ],
        "runtime_cross_module_package_interop_source_surface": surfaces[
            "runtime_cross_module_package_interop_source_surface"
        ],
        "runtime_textual_binary_interface_parity_source_surface": surfaces[
            "runtime_textual_binary_interface_parity_source_surface"
        ],
        "runtime_mixed_image_compatibility_interop_semantics_surface": surfaces[
            "runtime_mixed_image_compatibility_interop_semantics_surface"
        ],
        "runtime_package_loading_module_identity_semantics_surface": surfaces[
            "runtime_package_loading_module_identity_semantics_surface"
        ],
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": surfaces[
            "runtime_c_cpp_swift_bridge_compatibility_semantics_surface"
        ],
        "runtime_import_version_feature_claim_diagnostics_surface": surfaces[
            "runtime_import_version_feature_claim_diagnostics_surface"
        ],
        "runtime_packaging_bridge_loader_artifact_surface": surfaces[
            "runtime_packaging_bridge_loader_artifact_surface"
        ],
        "runtime_mixed_image_package_lowering_bridge_emission_surface": surfaces[
            "runtime_mixed_image_package_lowering_bridge_emission_surface"
        ],
        "runtime_cross_language_replay_import_surface_preservation_surface": surfaces[
            "runtime_cross_language_replay_import_surface_preservation_surface"
        ],
        "runtime_package_loader_bridge_abi_surface": surfaces[
            "runtime_package_loader_bridge_abi_surface"
        ],
        "runtime_package_loading_interop_implementation_surface": surfaces[
            "runtime_package_loading_interop_implementation_surface"
        ],
        "runtime_property_atomicity_synthesis_reflection_source_surface": surfaces[
            "runtime_property_atomicity_synthesis_reflection_source_surface"
        ],
        "runtime_realization_lowering_reflection_artifact_surface": surfaces[
            "runtime_realization_lowering_reflection_artifact_surface"
        ],
        "runtime_dispatch_table_reflection_record_lowering_surface": surfaces[
            "runtime_dispatch_table_reflection_record_lowering_surface"
        ],
        "runtime_cross_module_realized_metadata_replay_preservation_surface": surfaces[
            "runtime_cross_module_realized_metadata_replay_preservation_surface"
        ],
        "runtime_object_model_abi_query_surface": surfaces[
            "runtime_object_model_abi_query_surface"
        ],
        "runtime_realization_lookup_reflection_implementation_surface": surfaces[
            "runtime_realization_lookup_reflection_implementation_surface"
        ],
        "runtime_reflection_query_surface": surfaces["runtime_reflection_query_surface"],
        "runtime_realization_lookup_semantics_surface": surfaces[
            "runtime_realization_lookup_semantics_surface"
        ],
        "runtime_class_metaclass_protocol_realization_surface": surfaces[
            "runtime_class_metaclass_protocol_realization_surface"
        ],
        "runtime_category_attachment_merged_dispatch_surface": surfaces[
            "runtime_category_attachment_merged_dispatch_surface"
        ],
        "runtime_reflection_visibility_coherence_diagnostics_surface": surfaces[
            "runtime_reflection_visibility_coherence_diagnostics_surface"
        ],
    }
    for optional_surface_key in (
        "runtime_installation_abi_surface",
        "runtime_loader_lifecycle_surface",
    ):
        optional_surface = report.get(optional_surface_key)
        if isinstance(optional_surface, dict):
            payload[optional_surface_key] = optional_surface
    if entry.suite_id == "runtime-acceptance":
        payload["case_count"] = report.get("case_count", 0)
    else:
        payload["step_count"] = len(report.get("steps", []))
        payload["runner_path"] = report.get("runner_path")
    return payload


__all__ = ["summarize_report"]
