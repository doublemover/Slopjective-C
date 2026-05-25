#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"
#include "lower/contracts/metaprogramming_runtime_cache_contracts.h"

struct Objc3Program;

namespace objc3::artifacts::frontend {

struct Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary {
  std::string contract_id =
      kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationContractId;
  std::string source_contract_id =
      kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationSourceContractId;
  std::string surface_path =
      kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationSourceModel;
  std::string preservation_model =
      kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationModel;
  std::string fail_closed_model =
      kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationFailClosedModel;
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
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId;
  std::string source_contract_id =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId;
  std::string surface_path =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName;
  std::string host_executable_relative_path =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath;
  std::string cache_root_relative_path =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath;
  std::string host_model =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel;
  std::string toolchain_model =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel;
  std::string cache_model =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel;
  std::string invalidation_model =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationInvalidationModel;
  std::string sandbox_policy_model =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSandboxPolicyModel;
  std::string diagnostics_model =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationDiagnosticsModel;
  std::string fail_closed_model =
      kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel;
  std::string macro_package_identity =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationPackageIdentity;
  std::string macro_package_lock_identity =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationPackageLockIdentity;
  std::string macro_package_trust_identity =
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationPackageTrustIdentity;
  std::string macro_input_content_identity;
  std::string macro_output_content_identity;
  std::string macro_host_identity;
  std::string cache_validation_status;
  std::string runtime_consumption_artifact_identity;
  std::string replay_key;
  std::string metaprogramming_replay_key;
  std::size_t package_replay_generation = 0;
  std::size_t local_macro_artifact_count = 0;
  std::size_t local_property_behavior_artifact_count = 0;
  std::size_t imported_module_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_ready = false;
  bool deterministic = false;
};

[[nodiscard]] Objc3MetaprogrammingExpansionLoweringContract
BuildMetaprogrammingExpansionLoweringContract(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary);

[[nodiscard]] std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
BuildMetaprogrammingDerivedMethodBundles(const Objc3Program &program);

[[nodiscard]] std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
BuildMetaprogrammingMacroArtifactBundles(const Objc3Program &program);

[[nodiscard]] std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
BuildMetaprogrammingPropertyBehaviorArtifactBundles(const Objc3Program &program);

[[nodiscard]] Objc3MetaprogrammingSynthesizedArtifactEmissionContract
BuildMetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
        &derive_bundles,
    const std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> &macro_bundles,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles);

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
