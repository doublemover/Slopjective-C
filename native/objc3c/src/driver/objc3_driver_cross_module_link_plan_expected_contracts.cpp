#include "driver/objc3_driver_cross_module_link_plan_expected_contracts.h"

#include "ast/objc3_ast.h"
#include "io/objc3_manifest_artifacts.h"
#include "lower/contracts/lowering_arc_contracts.h"
#include "lower/objc3_lowering_contract.h"

void PopulateObjc3DriverCrossModuleRuntimeLinkPlanExpectedContracts(
    Objc3CrossModuleRuntimeLinkPlanArtifactInputs &link_plan_inputs,
    const Objc3FrontendArtifactBundle &artifacts) {
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
  link_plan_inputs
      .expected_block_ownership_retain_release_operation_lowering_contract_id =
      kObjc3RetainReleaseOperationLoweringLaneContract;
  link_plan_inputs
      .expected_block_ownership_autoreleasepool_scope_lowering_contract_id =
      kObjc3AutoreleasePoolScopeLoweringLaneContract;
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
}
