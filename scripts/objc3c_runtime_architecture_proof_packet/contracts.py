"""Runtime architecture proof-packet contracts and stable report paths."""

from __future__ import annotations

from objc3c_tooling.paths import ROOT


HARNESS_SCRIPT = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
HARNESS_SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "runtime"
    / "shared-executable-acceptance-harness"
    / "public-test-smoke"
    / "summary.json"
)
PROOF_PACKET_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "architecture-proof" / "summary.json"
)
PUBLIC_SMOKE_SUITE_ID = "public-test-smoke"
PROOF_PACKET_CONTRACT_ID = "objc3c.runtime.architecture.proof.packet.v1"
PROOF_PACKET_SURFACE_CONTRACT_ID = "objc3c.runtime.architecture.proof.packet.surface.v1"
HARNESS_SUMMARY_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.summary.v1"
CLAIM_BOUNDARY_CONTRACT_ID = "objc3c.runtime.execution.claim.boundary.v1"
SURFACE_KEYS = (
    "runtime_state_publication_surface",
    "acceptance_suite_surface",
    "runtime_error_execution_cleanup_source_surface",
    "runtime_catch_filter_finalization_source_surface",
    "runtime_error_propagation_cleanup_semantics_surface",
    "runtime_bridging_filter_unwind_diagnostics_surface",
    "runtime_error_lowering_unwind_bridge_helper_surface",
    "runtime_error_runtime_abi_cleanup_surface",
    "runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
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
    "runtime_installation_abi_surface",
    "runtime_loader_lifecycle_surface",
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
)
PACKET_SURFACE_KEYS = (
    "runtime_state_publication_surface",
    "acceptance_suite_surface",
    "runtime_error_execution_cleanup_source_surface",
    "runtime_catch_filter_finalization_source_surface",
    "runtime_error_propagation_cleanup_semantics_surface",
    "runtime_bridging_filter_unwind_diagnostics_surface",
    "runtime_error_lowering_unwind_bridge_helper_surface",
    "runtime_error_runtime_abi_cleanup_surface",
    "runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
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
    "runtime_installation_abi_surface",
    "runtime_loader_lifecycle_surface",
)
