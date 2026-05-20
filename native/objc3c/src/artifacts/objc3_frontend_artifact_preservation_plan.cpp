#include "artifacts/objc3_frontend_artifact_preservation_plan.h"

namespace {

using objc3::artifacts::frontend::
    BuildDispatchDispatchMetadataInterfacePreservationSummary;
using objc3::artifacts::frontend::
    BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary;
using objc3::artifacts::frontend::
    BuildMetaprogrammingModuleInterfaceReplayPreservationSummary;

}  // namespace

Objc3FrontendArtifactPreservationPlan
BuildObjc3FrontendArtifactPreservationPlan(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const Objc3RetainReleaseOperationLoweringContract
        &retain_release_operation_lowering_contract,
    const std::string &retain_release_operation_lowering_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract
        &autoreleasepool_scope_lowering_contract,
    const std::string &autoreleasepool_scope_lowering_replay_key,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3MetaprogrammingExpansionLoweringContract
        &metaprogramming_expansion_lowering_contract,
    const std::string &metaprogramming_expansion_lowering_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &metaprogramming_synthesized_artifact_emission_contract,
    const std::string
        &metaprogramming_synthesized_artifact_emission_replay_key,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &metaprogramming_property_behavior_artifact_bundles,
    const Objc3FrontendOptions &options) {
  Objc3FrontendArtifactPreservationPlan plan;
  plan.dispatch_dispatch_metadata_interface_preservation_summary =
      BuildDispatchDispatchMetadataInterfacePreservationSummary(
          runtime_metadata_source_records,
          dispatch_dispatch_control_lowering_replay_key,
          runtime_import_artifact_ready,
          imported_runtime_module_surfaces);
  plan.runtime_block_ownership_artifact_preservation_summary =
      BuildObjc3RuntimeBlockOwnershipArtifactPreservationSummary(
          block_abi_invoke_trampoline_lowering_contract,
          block_storage_escape_lowering_contract,
          block_copy_dispose_lowering_contract,
          retain_release_operation_lowering_contract,
          retain_release_operation_lowering_replay_key,
          autoreleasepool_scope_lowering_contract,
          autoreleasepool_scope_lowering_replay_key,
          runtime_support_library_link_wiring);
  plan.runtime_storage_reflection_artifact_preservation_summary =
      BuildObjc3RuntimeStorageReflectionArtifactPreservationSummary(
          runtime_metadata_source_records);
  plan.metaprogramming_module_interface_replay_preservation_summary =
      BuildMetaprogrammingModuleInterfaceReplayPreservationSummary(
          metaprogramming_expansion_lowering_contract,
          metaprogramming_expansion_lowering_replay_key,
          metaprogramming_synthesized_artifact_emission_contract,
          metaprogramming_synthesized_artifact_emission_replay_key,
          metaprogramming_property_behavior_artifact_bundles,
          runtime_import_artifact_ready,
          imported_runtime_module_surfaces);
  plan.metaprogramming_macro_host_process_cache_runtime_integration_summary =
      BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary(
          plan.metaprogramming_module_interface_replay_preservation_summary,
          imported_runtime_module_surfaces,
          options);
  return plan;
}
