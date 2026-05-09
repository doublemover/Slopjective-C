#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/metaprogramming_expansion_lowering_contracts.h"
#include "lower/contracts/metaprogramming_replay_preservation_contracts.h"
#include "lower/contracts/metaprogramming_runtime_cache_contracts.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "pipeline/results/compile_options.h"

namespace objc3::artifacts::frontend {

struct Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary {
  std::string contract_id =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId;
  std::string source_contract_id =
      kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId;
  std::string surface_path =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationSourceModel;
  std::string preservation_model =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationModel;
  std::string fail_closed_model =
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationFailClosedModel;
  std::string replay_key;
  std::string expansion_lowering_replay_key;
  std::string synthesized_emission_replay_key;
  std::vector<std::string> imported_module_names_lexicographic;
  std::size_t local_derive_method_count = 0;
  std::size_t local_macro_artifact_count = 0;
  std::size_t local_interface_property_behavior_artifact_count = 0;
  std::size_t local_implementation_property_behavior_artifact_count = 0;
  std::size_t local_runtime_method_list_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_derive_method_count = 0;
  std::size_t imported_macro_artifact_count = 0;
  std::size_t imported_interface_property_behavior_artifact_count = 0;
  std::size_t imported_implementation_property_behavior_artifact_count = 0;
  std::size_t imported_runtime_method_list_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

struct Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary {
  std::string contract_id =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId;
  std::string source_contract_id =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId;
  std::string surface_path =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName;
  std::string host_executable_relative_path =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath;
  std::string cache_root_relative_path =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath;
  std::string host_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel;
  std::string toolchain_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel;
  std::string cache_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel;
  std::string fail_closed_model =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel;
  std::string replay_key;
  std::string metaprogramming_replay_key;
  std::size_t local_macro_artifact_count = 0;
  std::size_t local_property_behavior_artifact_count = 0;
  std::size_t imported_module_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_ready = false;
  bool deterministic = false;
};

[[nodiscard]] std::string
BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary);

[[nodiscard]] std::string
BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary);

[[nodiscard]] std::string
BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary);

[[nodiscard]] std::string
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &summary);

[[nodiscard]] std::string BuildMetaprogrammingExpansionLoweringContractJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary,
    const Objc3MetaprogrammingExpansionLoweringContract &contract,
    const std::string &replay_key);

[[nodiscard]] std::string
BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract,
    const std::string &replay_key);

[[nodiscard]] Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
BuildMetaprogrammingModuleInterfaceReplayPreservationSummary(
    const Objc3MetaprogrammingExpansionLoweringContract &expansion_contract,
    const std::string &expansion_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &synthesized_contract,
    const std::string &synthesized_replay_key,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces);

[[nodiscard]] std::string
BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &summary);

[[nodiscard]]
Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &module_interface_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    const Objc3FrontendOptions &options);

[[nodiscard]] std::string
BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
    const Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
        &summary);

}  // namespace objc3::artifacts::frontend
