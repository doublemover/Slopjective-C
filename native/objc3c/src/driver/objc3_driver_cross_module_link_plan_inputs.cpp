#include "driver/objc3_driver_cross_module_link_plan_inputs.h"

#include <filesystem>

#include "ast/objc3_ast.h"
#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"

namespace fs = std::filesystem;

Objc3CrossModuleRuntimeLinkPlanArtifactInputs
BuildObjc3DriverCrossModuleRuntimeLinkPlanInputs(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const std::filesystem::path &object_out) {
  Objc3CrossModuleRuntimeLinkPlanArtifactInputs link_plan_inputs;
  link_plan_inputs.contract_id = kObjc3CrossModuleRuntimeLinkPlanContractId;
  link_plan_inputs.source_orchestration_contract_id =
      kObjc3CrossModuleBuildRuntimeOrchestrationContractId;
  link_plan_inputs.import_surface_contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  link_plan_inputs.registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  link_plan_inputs.payload_model = kObjc3CrossModuleRuntimeLinkPlanPayloadModel;
  link_plan_inputs.artifact_relative_path =
      kObjc3CrossModuleRuntimeLinkPlanArtifactRelativePath;
  link_plan_inputs.linker_response_artifact_relative_path =
      kObjc3CrossModuleRuntimeLinkerResponseArtifactRelativePath;
  link_plan_inputs.authority_model =
      kObjc3CrossModuleRuntimeLinkPlanAuthorityModel;
  link_plan_inputs.packaging_model =
      kObjc3CrossModuleRuntimeLinkPlanPackagingModel;
  link_plan_inputs.registration_scope_model =
      kObjc3CrossModuleRuntimeLinkPlanRegistrationScopeModel;
  link_plan_inputs.link_object_order_model =
      kObjc3CrossModuleRuntimeLinkObjectOrderModel;
  link_plan_inputs.local_module_name =
      artifacts.runtime_aware_import_module_frontend_closure_summary.module_name;
  link_plan_inputs.local_import_surface_artifact_relative_path =
      fs::absolute(BuildRuntimeAwareImportModuleArtifactPath(
                       cli_options.out_dir, cli_options.emit_prefix))
          .lexically_normal()
          .generic_string();
  link_plan_inputs.local_registration_manifest_artifact_relative_path =
      fs::absolute(BuildRuntimeRegistrationManifestArtifactPath(
                       cli_options.out_dir, cli_options.emit_prefix))
          .lexically_normal()
          .generic_string();
  link_plan_inputs.local_object_artifact_relative_path =
      fs::absolute(object_out).lexically_normal().generic_string();
  link_plan_inputs.runtime_support_library_archive_relative_path =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .runtime_support_library_archive_relative_path;
  link_plan_inputs.object_format = linker_retention_artifacts.object_format;
  link_plan_inputs.local_translation_unit_identity_model =
      linker_retention_artifacts.translation_unit_identity_model;
  link_plan_inputs.local_translation_unit_identity_key =
      linker_retention_artifacts.translation_unit_identity_key;
  link_plan_inputs.local_translation_unit_registration_order_ordinal =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .translation_unit_registration_order_ordinal;
  link_plan_inputs.local_class_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .class_descriptor_count;
  link_plan_inputs.local_protocol_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .protocol_descriptor_count;
  link_plan_inputs.local_category_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .category_descriptor_count;
  link_plan_inputs.local_property_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .property_descriptor_count;
  link_plan_inputs.local_ivar_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .ivar_descriptor_count;
  link_plan_inputs.local_total_descriptor_count =
      artifacts.runtime_translation_unit_registration_manifest_summary
          .total_descriptor_count;

  const auto local_block_ownership_summary =
      artifacts.runtime_block_ownership_artifact_preservation_summary;
  link_plan_inputs.local_block_ownership_block_literal_sites =
      local_block_ownership_summary.local_block_literal_sites;
  link_plan_inputs.local_block_ownership_invoke_trampoline_symbolized_sites =
      local_block_ownership_summary.local_invoke_trampoline_symbolized_sites;
  link_plan_inputs.local_block_ownership_copy_helper_required_sites =
      local_block_ownership_summary.local_copy_helper_required_sites;
  link_plan_inputs.local_block_ownership_dispose_helper_required_sites =
      local_block_ownership_summary.local_dispose_helper_required_sites;
  link_plan_inputs.local_block_ownership_copy_helper_symbolized_sites =
      local_block_ownership_summary.local_copy_helper_symbolized_sites;
  link_plan_inputs.local_block_ownership_dispose_helper_symbolized_sites =
      local_block_ownership_summary.local_dispose_helper_symbolized_sites;
  link_plan_inputs.local_block_ownership_escape_to_heap_sites =
      local_block_ownership_summary.local_escape_to_heap_sites;
  link_plan_inputs.local_block_ownership_byref_layout_symbolized_sites =
      local_block_ownership_summary.local_byref_layout_symbolized_sites;

  const auto local_storage_reflection_summary =
      artifacts.runtime_storage_reflection_artifact_preservation_summary;
  link_plan_inputs
      .local_storage_reflection_implementation_owned_property_entries =
      local_storage_reflection_summary.implementation_owned_property_entries;
  link_plan_inputs.local_storage_reflection_synthesized_accessor_owner_entries =
      local_storage_reflection_summary.synthesized_accessor_owner_entries;
  link_plan_inputs.local_storage_reflection_synthesized_getter_entries =
      local_storage_reflection_summary.synthesized_getter_entries;
  link_plan_inputs.local_storage_reflection_synthesized_setter_entries =
      local_storage_reflection_summary.synthesized_setter_entries;
  link_plan_inputs.local_storage_reflection_synthesized_accessor_entries =
      local_storage_reflection_summary.synthesized_accessor_entries;
  link_plan_inputs.local_storage_reflection_current_property_read_entries =
      local_storage_reflection_summary.current_property_read_entries;
  link_plan_inputs.local_storage_reflection_current_property_write_entries =
      local_storage_reflection_summary.current_property_write_entries;
  link_plan_inputs.local_storage_reflection_current_property_exchange_entries =
      local_storage_reflection_summary.current_property_exchange_entries;
  link_plan_inputs.local_storage_reflection_weak_current_property_load_entries =
      local_storage_reflection_summary.weak_current_property_load_entries;
  link_plan_inputs.local_storage_reflection_weak_current_property_store_entries =
      local_storage_reflection_summary.weak_current_property_store_entries;
  link_plan_inputs.local_storage_reflection_ivar_layout_entries =
      local_storage_reflection_summary.ivar_layout_entries;
  link_plan_inputs.local_storage_reflection_ivar_layout_owner_entries =
      local_storage_reflection_summary.ivar_layout_owner_entries;
  link_plan_inputs.local_driver_linker_flags = {
      linker_retention_artifacts.driver_linker_flag};

  link_plan_inputs.expected_error_handling_contract_id =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId;
  link_plan_inputs.expected_error_handling_source_contract_id =
      kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId;
  link_plan_inputs.expected_concurrency_actor_contract_id =
      "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1";
  link_plan_inputs.expected_concurrency_actor_source_contract_id =
      "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
  link_plan_inputs.expected_interop_ffi_contract_id =
      kObjc3InteropFfiMetadataInterfacePreservationContractId;
  link_plan_inputs.expected_interop_ffi_source_contract_id =
      kObjc3InteropFfiMetadataInterfacePreservationSourceContractId;
  link_plan_inputs.expected_interop_ffi_preservation_contract_id =
      kObjc3InteropForeignSurfaceInterfacePreservationContractId;
  link_plan_inputs.expected_interop_header_module_bridge_contract_id =
      kObjc3InteropHeaderModuleBridgeGenerationContractId;
  link_plan_inputs.expected_interop_header_module_bridge_source_contract_id =
      kObjc3InteropHeaderModuleBridgeGenerationSourceContractId;
  link_plan_inputs
      .expected_interop_header_module_bridge_preservation_contract_id =
      kObjc3InteropHeaderModuleBridgeGenerationPreservationContractId;
  link_plan_inputs.expected_interop_bridge_header_artifact_relative_path =
      kObjc3InteropBridgeHeaderArtifactRelativePath;
  link_plan_inputs.expected_interop_bridge_module_artifact_relative_path =
      kObjc3InteropBridgeModuleArtifactRelativePath;
  link_plan_inputs.expected_interop_bridge_artifact_relative_path =
      kObjc3InteropBridgeArtifactRelativePath;
  link_plan_inputs.expected_metaprogramming_host_cache_contract_id =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId;
  link_plan_inputs.expected_metaprogramming_host_cache_source_contract_id =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId;
  link_plan_inputs.expected_metaprogramming_host_cache_executable_relative_path =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath;
  link_plan_inputs.expected_metaprogramming_host_cache_root_relative_path =
      artifacts
          .metaprogramming_macro_host_process_cache_runtime_integration_cache_root_relative_path;
  link_plan_inputs.expected_block_ownership_contract_id =
      kObjc3RuntimeBlockOwnershipArtifactPreservationContractId;
  link_plan_inputs.expected_block_ownership_source_contract_id =
      kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId;
  link_plan_inputs
      .expected_block_ownership_object_invoke_thunk_lowering_contract_id =
      Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId;
  link_plan_inputs.expected_block_ownership_byref_helper_lowering_contract_id =
      Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId;
  link_plan_inputs
      .expected_block_ownership_escape_runtime_hook_lowering_contract_id =
      Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId;
  link_plan_inputs
      .expected_block_ownership_runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  link_plan_inputs.expected_storage_reflection_contract_id =
      kObjc3RuntimeStorageReflectionArtifactPreservationContractId;
  link_plan_inputs.expected_storage_reflection_source_contract_id =
      kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId;
  link_plan_inputs
      .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
      kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId;
  link_plan_inputs
      .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id =
      kObjc3ExecutablePropertyAccessorLayoutLoweringContractId;
  link_plan_inputs
      .expected_storage_reflection_executable_ivar_layout_emission_contract_id =
      kObjc3ExecutableIvarLayoutEmissionContractId;
  link_plan_inputs
      .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
      kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId;
  link_plan_inputs.expected_bootstrap_live_registration_contract_id =
      "objc3c.runtime.live.registration.discovery.replay.v1";
  link_plan_inputs.expected_bootstrap_live_restart_hardening_contract_id =
      "objc3c.runtime.live.restart.hardening.v1";
  link_plan_inputs.expected_bootstrap_replay_registered_images_symbol =
      kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
  link_plan_inputs.expected_bootstrap_reset_replay_state_snapshot_symbol =
      kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
  link_plan_inputs.expected_bootstrap_reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;

  return link_plan_inputs;
}
