#include "artifacts/objc3_frontend_artifact_bundle_outputs.h"

#include "artifacts/objc3_frontend_artifacts.h"

namespace objc3::artifacts::frontend {
namespace {

void PopulateObjc3FrontendArtifactBundleSummaryOutputs(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &runtime_aware_import_module_frontend_closure,
    const Objc3VersionedConformanceReportLoweringSummary
        &versioned_conformance_report_lowering,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBlockOwnershipArtifactPreservationSummary
        &runtime_block_ownership_artifact_preservation_summary,
    const Objc3RuntimeStorageReflectionArtifactPreservationSummary
        &runtime_storage_reflection_artifact_preservation_summary,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapLegalityFailureContractSummary
        &runtime_bootstrap_legality_failure_contract,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &frontend_compatibility_strictness_claim_semantics,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &tooling_legacy_canonical_migration_semantics_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &tooling_machine_readable_conformance_report_contract_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &tooling_feature_aware_conformance_report_emission_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &tooling_corpus_sharding_release_evidence_packaging_summary,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering) {
  bundle.runtime_aware_import_module_frontend_closure_summary =
      runtime_aware_import_module_frontend_closure;
  bundle.versioned_conformance_report_lowering_summary =
      versioned_conformance_report_lowering;
  bundle.runtime_registration_descriptor_image_root_source_surface_summary =
      runtime_registration_descriptor_image_root_source_surface;
  bundle.runtime_registration_descriptor_frontend_closure_summary =
      runtime_registration_descriptor_frontend_closure;
  bundle.runtime_block_ownership_artifact_preservation_summary =
      runtime_block_ownership_artifact_preservation_summary;
  bundle.runtime_storage_reflection_artifact_preservation_summary =
      runtime_storage_reflection_artifact_preservation_summary;
  bundle.runtime_translation_unit_registration_manifest_summary =
      runtime_translation_unit_registration_manifest;
  bundle.runtime_bootstrap_legality_semantics_summary =
      runtime_bootstrap_legality_semantics;
  bundle.runtime_bootstrap_legality_failure_contract_summary =
      runtime_bootstrap_legality_failure_contract;
  bundle.runtime_bootstrap_failure_restart_semantics_summary =
      runtime_bootstrap_failure_restart_semantics;
  bundle.frontend_compatibility_strictness_claim_semantics_summary =
      frontend_compatibility_strictness_claim_semantics;
  bundle.tooling_legacy_canonical_migration_semantics_summary =
      tooling_legacy_canonical_migration_semantics_summary;
  bundle.tooling_machine_readable_conformance_report_contract_summary =
      tooling_machine_readable_conformance_report_contract_summary;
  bundle.tooling_feature_aware_conformance_report_emission_summary =
      tooling_feature_aware_conformance_report_emission_summary;
  bundle.tooling_corpus_sharding_release_evidence_packaging_summary =
      tooling_corpus_sharding_release_evidence_packaging_summary;
  bundle.runtime_bootstrap_api_summary = runtime_bootstrap_api;
  bundle.runtime_bootstrap_semantics_summary = runtime_bootstrap_semantics;
  bundle.runtime_bootstrap_lowering_summary = runtime_bootstrap_lowering;
}

}  // namespace

void PopulateObjc3FrontendArtifactBundleOutputs(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &runtime_aware_import_module_frontend_closure,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    const Objc3TypeSystemOptionalKeypathLoweringContract
        &type_system_optional_keypath_lowering_contract,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary,
    const std::string &message_send_selector_lowering_replay_key,
    const std::string &dispatch_abi_marshalling_replay_key,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const std::string &type_system_optional_keypath_lowering_replay_key,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const objc3::artifacts::evidence::
        ErrorHandlingResultAndBridgingArtifactReplayEvidence
        &error_handling_result_and_bridging_artifact_replay_summary,
    const Objc3ActorLoweringMetadataContract
        &concurrency_actor_lowering_metadata_contract,
    const std::string &concurrency_actor_lowering_metadata_replay_key,
    const std::string
        &concurrency_actor_isolation_sendability_lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &interop_foreign_surface_interface_preservation_summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary
        &interop_header_module_bridge_generation_summary,
    const Objc3InteropForeignCallLifetimeLoweringContract
        &interop_foreign_call_lifetime_lowering_contract,
    const std::string &interop_foreign_call_lifetime_lowering_replay_key,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &interop_ffi_metadata_interface_preservation_contract,
    const std::string &interop_ffi_metadata_interface_preservation_replay_key,
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &metaprogramming_module_interface_replay_preservation_summary,
    const Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
        &metaprogramming_macro_host_process_cache_runtime_integration_summary,
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &dispatch_dispatch_metadata_interface_preservation_summary,
    const Objc3RuntimeBlockOwnershipArtifactPreservationSummary
        &runtime_block_ownership_artifact_preservation_summary,
    const Objc3RuntimeStorageReflectionArtifactPreservationSummary
        &runtime_storage_reflection_artifact_preservation_summary,
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3RuntimeMetadataSourceRecordSet
        &serialized_runtime_metadata_reuse_records,
    const Objc3VersionedConformanceReportLoweringSummary
        &versioned_conformance_report_lowering,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &frontend_compatibility_strictness_claim_semantics,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &tooling_feature_aware_conformance_report_emission_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &tooling_corpus_sharding_release_evidence_packaging_summary,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapLegalityFailureContractSummary
        &runtime_bootstrap_legality_failure_contract,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &tooling_legacy_canonical_migration_semantics_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &tooling_machine_readable_conformance_report_contract_summary,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering) {
  PopulateObjc3FrontendRuntimeImportArtifactOutputs(
      bundle, program, runtime_aware_import_module_frontend_closure,
      runtime_metadata_source_records, type_metadata_handoff,
      type_system_optional_keypath_lowering_contract,
      type_system_type_semantic_model_summary,
      message_send_selector_lowering_replay_key,
      dispatch_abi_marshalling_replay_key,
      nil_receiver_semantics_foldability_replay_key,
      type_system_optional_keypath_lowering_replay_key,
      runtime_support_library_link_wiring,
      error_handling_result_and_bridging_artifact_replay_summary,
      concurrency_actor_lowering_metadata_contract,
      concurrency_actor_lowering_metadata_replay_key,
      concurrency_actor_isolation_sendability_lowering_replay_key,
      interop_foreign_surface_interface_preservation_summary,
      interop_header_module_bridge_generation_summary,
      interop_foreign_call_lifetime_lowering_contract,
      interop_foreign_call_lifetime_lowering_replay_key,
      interop_ffi_metadata_interface_preservation_contract,
      interop_ffi_metadata_interface_preservation_replay_key,
      metaprogramming_module_interface_replay_preservation_summary,
      metaprogramming_macro_host_process_cache_runtime_integration_summary,
      dispatch_dispatch_metadata_interface_preservation_summary,
      runtime_block_ownership_artifact_preservation_summary,
      runtime_storage_reflection_artifact_preservation_summary,
      serialized_runtime_metadata_artifact_reuse,
      serialized_runtime_metadata_reuse_records);
  objc3::artifacts::reports::
      PopulateObjc3FrontendVersionedConformanceReportOutput(
          bundle, versioned_conformance_report_lowering, options,
          pipeline_result, frontend_compatibility_strictness_claim_semantics,
          tooling_feature_aware_conformance_report_emission_summary,
          tooling_corpus_sharding_release_evidence_packaging_summary);
  PopulateObjc3FrontendArtifactBundleSummaryOutputs(
      bundle, runtime_aware_import_module_frontend_closure,
      versioned_conformance_report_lowering,
      runtime_registration_descriptor_image_root_source_surface,
      runtime_registration_descriptor_frontend_closure,
      runtime_block_ownership_artifact_preservation_summary,
      runtime_storage_reflection_artifact_preservation_summary,
      runtime_translation_unit_registration_manifest,
      runtime_bootstrap_legality_semantics,
      runtime_bootstrap_legality_failure_contract,
      runtime_bootstrap_failure_restart_semantics,
      frontend_compatibility_strictness_claim_semantics,
      tooling_legacy_canonical_migration_semantics_summary,
      tooling_machine_readable_conformance_report_contract_summary,
      tooling_feature_aware_conformance_report_emission_summary,
      tooling_corpus_sharding_release_evidence_packaging_summary,
      runtime_bootstrap_api, runtime_bootstrap_semantics,
      runtime_bootstrap_lowering);
}

}  // namespace objc3::artifacts::frontend
