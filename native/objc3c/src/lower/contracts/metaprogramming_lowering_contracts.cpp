#include "lower/contracts/metaprogramming_lowering_contracts.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

bool IsValidObjc3MetaprogrammingExpansionLoweringContract(
    const Objc3MetaprogrammingExpansionLoweringContract &contract) {
  if (contract.derived_selector_artifact_sites > contract.derive_inventory_sites ||
      contract.synthesized_binding_sites > contract.property_behavior_sites ||
      contract.synthesized_getter_sites > contract.property_behavior_sites ||
      contract.synthesized_setter_sites > contract.property_behavior_sites) {
    return false;
  }
  const std::size_t required_replay_visible_sites =
      contract.derived_selector_artifact_sites +
      contract.macro_replay_visible_sites + contract.property_behavior_sites +
      contract.synthesized_binding_sites + contract.synthesized_getter_sites +
      contract.synthesized_setter_sites;
  if (contract.replay_visible_metadata_sites != required_replay_visible_sites) {
    return false;
  }
  if (contract.contract_violation_sites > contract.guard_blocked_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3MetaprogrammingExpansionLoweringReplayKey(
    const Objc3MetaprogrammingExpansionLoweringContract &contract) {
  return std::string("derive_inventory_sites=") +
             std::to_string(contract.derive_inventory_sites) +
         ";derived_selector_artifact_sites=" +
         std::to_string(contract.derived_selector_artifact_sites) +
         ";macro_replay_visible_sites=" +
         std::to_string(contract.macro_replay_visible_sites) +
         ";property_behavior_sites=" +
         std::to_string(contract.property_behavior_sites) +
         ";synthesized_binding_sites=" +
         std::to_string(contract.synthesized_binding_sites) +
         ";synthesized_getter_sites=" +
         std::to_string(contract.synthesized_getter_sites) +
         ";synthesized_setter_sites=" +
         std::to_string(contract.synthesized_setter_sites) +
         ";replay_visible_metadata_sites=" +
         std::to_string(contract.replay_visible_metadata_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MetaprogrammingExpansionLoweringLaneContract;
}

bool IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract) {
  if (contract.emitted_derive_method_sites > contract.derive_inventory_sites) {
    return false;
  }
  if (contract.emitted_global_artifact_sites <
      contract.emitted_macro_artifact_sites +
          contract.emitted_property_behavior_artifact_sites) {
    return false;
  }
  if (contract.emitted_runtime_method_list_sites <
      contract.emitted_derive_method_sites) {
    return false;
  }
  if (contract.contract_violation_sites > contract.guard_blocked_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3MetaprogrammingSynthesizedArtifactEmissionReplayKey(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract) {
  return std::string("derive_inventory_sites=") +
             std::to_string(contract.derive_inventory_sites) +
         ";emitted_derive_method_sites=" +
         std::to_string(contract.emitted_derive_method_sites) +
         ";emitted_macro_artifact_sites=" +
         std::to_string(contract.emitted_macro_artifact_sites) +
         ";emitted_property_behavior_artifact_sites=" +
         std::to_string(contract.emitted_property_behavior_artifact_sites) +
         ";emitted_global_artifact_sites=" +
         std::to_string(contract.emitted_global_artifact_sites) +
         ";emitted_runtime_method_list_sites=" +
         std::to_string(contract.emitted_runtime_method_list_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract;
}

std::string Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId
      << ";source_contract="
      << kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId
      << ";surface_path="
      << kObjc3MetaprogrammingModuleInterfaceReplayPreservationSurfacePath
      << ";artifact_member="
      << kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName
      << ";source_model="
      << kObjc3MetaprogrammingModuleInterfaceReplayPreservationSourceModel
      << ";preservation_model="
      << kObjc3MetaprogrammingModuleInterfaceReplayPreservationModel
      << ";fail_closed_model="
      << kObjc3MetaprogrammingModuleInterfaceReplayPreservationFailClosedModel
      << ";follow_on_surface=objc3c.metaprogramming.moduleinterface.replaypreservation.v1";
  return out.str();
}

std::string Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary() {
  std::ostringstream out;
  // host/runtime-boundary anchor: lane-D freezes one truthful Part 10
  // boundary over the already-landed synthesized/property runtime slice without
  // claiming live macro host execution or runtime package loading.
  out << "contract=" << kObjc3MetaprogrammingExpansionHostRuntimeBoundaryContractId
      << ";source_contract="
      << kObjc3MetaprogrammingExpansionHostRuntimeBoundarySourceContractId
      << ";host_model=" << kObjc3MetaprogrammingExpansionHostRuntimeBoundaryHostModel
      << ";property_runtime_model="
      << kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPropertyRuntimeModel
      << ";packaging_model="
      << kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPackagingModel
      << ";fail_closed_model="
      << kObjc3MetaprogrammingExpansionHostRuntimeBoundaryFailClosedModel
      << ";property_runtime_ready=true"
      << ";macro_host_execution_ready=false"
      << ";macro_host_process_launch_ready=false"
      << ";runtime_package_loader_ready=false"
      << ";deterministic=true"
      << ";follow_on_surface=objc3c.metaprogramming.runtimepackage.loader.v1";
  return out.str();
}

std::string Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId
      << ";source_contract="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId
      << ";surface_path="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath
      << ";artifact_member="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName
      << ";host_executable_relative_path="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath
      << ";cache_root_relative_path="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath
      << ";host_model="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel
      << ";toolchain_model="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel
      << ";cache_model="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel
      << ";invalidation_model="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationInvalidationModel
      << ";sandbox_policy_model="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSandboxPolicyModel
      << ";diagnostics_model="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationDiagnosticsModel
      << ";fail_closed_model="
      << kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel
      << ";macro_host_execution_ready=true"
      << ";macro_host_process_launch_ready=true"
      << ";runtime_package_loader_ready=false"
      << ";deterministic=true"
      << ";follow_on_surface=objc3c.metaprogramming.crossmodule.packaging.v1";
  return out.str();
}
