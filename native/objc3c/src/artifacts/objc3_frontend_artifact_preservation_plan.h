#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"
#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"
#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "pipeline/results/evidence_record.h"

struct Objc3FrontendArtifactPreservationPlan {
  objc3::artifacts::frontend::
      Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
      dispatch_dispatch_metadata_interface_preservation_summary;
  Objc3RuntimeBlockOwnershipArtifactPreservationSummary
      runtime_block_ownership_artifact_preservation_summary;
  Objc3RuntimeStorageReflectionArtifactPreservationSummary
      runtime_storage_reflection_artifact_preservation_summary;
  objc3::artifacts::frontend::
      Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
      metaprogramming_module_interface_replay_preservation_summary;
  objc3::artifacts::frontend::
      Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
      metaprogramming_macro_host_process_cache_runtime_integration_summary;
};

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
    const Objc3FrontendOptions &options);
