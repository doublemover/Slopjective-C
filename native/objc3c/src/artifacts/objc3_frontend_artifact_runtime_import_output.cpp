#include "artifacts/objc3_frontend_artifact_runtime_import_output.h"

#include "artifacts/interop/interop_bridge_artifacts.h"
#include "artifacts/objc3_frontend_artifacts.h"
#include "pipeline/results/evidence_record.h"

namespace objc3::artifacts::frontend {

std::string BuildObjc3FrontendRuntimeImportArtifactPayloadJson(
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
        &serialized_runtime_metadata_reuse_records) {
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          runtime_aware_import_module_frontend_closure)) {
    return {};
  }

  return RenderRuntimeAwareImportModuleArtifactJson(
      runtime_aware_import_module_frontend_closure,
      runtime_metadata_source_records,
      BuildTypeSystemOptionalKeypathLoweringContractJson(
          type_system_optional_keypath_lowering_contract,
          type_system_type_semantic_model_summary,
          type_system_type_semantic_model_summary.replay_key,
          message_send_selector_lowering_replay_key,
          dispatch_abi_marshalling_replay_key,
          nil_receiver_semantics_foldability_replay_key,
          type_system_optional_keypath_lowering_replay_key),
      BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
          type_system_optional_keypath_lowering_contract,
          runtime_support_library_link_wiring,
          type_system_optional_keypath_lowering_replay_key),
      BuildTypeSystemGenericContractPreservationJson(
          type_metadata_handoff, type_system_type_semantic_model_summary),
      BuildTypeSystemNullabilityContractPreservationJson(
          type_system_type_semantic_model_summary),
      BuildTypeSystemProtocolContractPreservationJson(
          program, runtime_metadata_source_records,
          type_system_type_semantic_model_summary),
      objc3::artifacts::evidence::
          BuildErrorHandlingResultAndBridgingArtifactReplayJson(
          error_handling_result_and_bridging_artifact_replay_summary),
      BuildConcurrencyActorMailboxRuntimeImportSummaryJson(
          BuildConcurrencyActorMailboxRuntimeImportSummary(
              concurrency_actor_lowering_metadata_contract,
              concurrency_actor_lowering_metadata_replay_key,
              concurrency_actor_isolation_sendability_lowering_replay_key)),
      BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
          interop_foreign_surface_interface_preservation_summary),
      BuildInteropHeaderModuleBridgeGenerationSummaryJson(
          interop_header_module_bridge_generation_summary),
      BuildInteropFfiMetadataInterfacePreservationContractJson(
          interop_foreign_call_lifetime_lowering_contract,
          interop_foreign_call_lifetime_lowering_replay_key,
          interop_foreign_surface_interface_preservation_summary,
          interop_ffi_metadata_interface_preservation_contract,
          interop_ffi_metadata_interface_preservation_replay_key),
      BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
          metaprogramming_module_interface_replay_preservation_summary),
      BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
          metaprogramming_macro_host_process_cache_runtime_integration_summary),
      BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
          dispatch_dispatch_metadata_interface_preservation_summary),
      BuildRuntimeBlockOwnershipArtifactPreservationSummaryJson(
          runtime_block_ownership_artifact_preservation_summary),
      BuildRuntimeStorageReflectionArtifactPreservationSummaryJson(
          runtime_storage_reflection_artifact_preservation_summary),
      serialized_runtime_metadata_artifact_reuse,
      serialized_runtime_metadata_reuse_records);
}

void PopulateObjc3FrontendRuntimeImportArtifactOutputs(
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
        &serialized_runtime_metadata_reuse_records) {
  bundle.runtime_aware_import_module_artifact_json =
      BuildObjc3FrontendRuntimeImportArtifactPayloadJson(
          program, runtime_aware_import_module_frontend_closure,
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

  if (interop_header_module_bridge_generation_summary.runtime_generation_ready &&
      interop_header_module_bridge_generation_summary.deterministic) {
    bundle.interop_bridge_header_artifact_text =
        interop::BuildInteropBridgeHeaderArtifactText(
            program, runtime_aware_import_module_frontend_closure,
            interop_header_module_bridge_generation_summary);
    bundle.interop_bridge_module_artifact_text =
        interop::BuildInteropBridgeModuleArtifactText(
            runtime_aware_import_module_frontend_closure,
            interop_header_module_bridge_generation_summary);
    bundle.interop_bridge_artifact_json = interop::BuildInteropBridgeArtifactJson(
        program, runtime_aware_import_module_frontend_closure,
        interop_header_module_bridge_generation_summary);
  }

  bundle.metaprogramming_macro_host_process_cache_runtime_integration_ready =
      metaprogramming_macro_host_process_cache_runtime_integration_summary
          .runtime_import_artifact_ready;
  bundle.metaprogramming_macro_host_process_cache_runtime_integration_replay_key =
      metaprogramming_macro_host_process_cache_runtime_integration_summary
          .replay_key;
  bundle
      .metaprogramming_macro_host_process_cache_runtime_integration_cache_root_relative_path =
      metaprogramming_macro_host_process_cache_runtime_integration_summary
          .cache_root_relative_path;

  if (error_handling_result_and_bridging_artifact_replay_summary
          .binary_artifact_replay_ready) {
    bundle.error_handling_result_bridge_artifact_replay_json =
        objc3::artifacts::evidence::
            BuildErrorHandlingResultAndBridgingArtifactReplayJson(
                error_handling_result_and_bridging_artifact_replay_summary);
  }
}

}  // namespace objc3::artifacts::frontend
