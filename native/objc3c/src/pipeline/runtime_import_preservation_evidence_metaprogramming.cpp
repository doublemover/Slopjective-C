#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <utility>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {
namespace {

bool PopulateImportedMetaprogrammingModuleInterfaceReplayPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value = FindMember(
      root, kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "metaprogramming module/interface replay preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.metaprogramming_runtime_import_artifact_ready,
                      error) ||
      !ReadBoolMember(
          *preservation_object, "separate_compilation_preservation_ready",
          surface.metaprogramming_separate_compilation_preservation_ready,
          error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.metaprogramming_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.metaprogramming_replay_key, error) ||
      !ReadStringMember(*preservation_object, "expansion_lowering_replay_key",
                        surface.metaprogramming_expansion_lowering_replay_key,
                        error) ||
      !ReadStringMember(*preservation_object, "synthesized_emission_replay_key",
                        surface.metaprogramming_synthesized_emission_replay_key,
                        error) ||
      !ReadSizeMember(*preservation_object, "local_derive_method_count",
                      surface.metaprogramming_local_derive_method_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_macro_artifact_count",
                      surface.metaprogramming_local_macro_artifact_count,
                      error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_interface_property_behavior_artifact_count",
          surface.metaprogramming_local_interface_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_implementation_property_behavior_artifact_count",
          surface
              .metaprogramming_local_implementation_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(*preservation_object, "local_runtime_method_list_count",
                      surface.metaprogramming_local_runtime_method_list_count,
                      error)) {
    return false;
  }

  if (contract_id !=
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation source contract id in import surface";
    return false;
  }

  surface.metaprogramming_module_interface_replay_preservation_present = true;
  surface.metaprogramming_contract_id = std::move(contract_id);
  surface.metaprogramming_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedMetaprogrammingMacroHostProcessCacheRuntimeIntegration(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *integration_value = FindMember(
      root,
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName);
  if (integration_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *integration_object =
      AsObject(*integration_value);
  if (integration_object == nullptr) {
    error =
        "metaprogramming macro host process/cache runtime integration surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*integration_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*integration_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*integration_object, "runtime_import_artifact_ready",
                      surface
                          .metaprogramming_macro_host_process_cache_runtime_ready,
                      error) ||
      !ReadBoolMember(
          *integration_object, "separate_compilation_ready",
          surface
              .metaprogramming_macro_host_process_cache_separate_compilation_ready,
          error) ||
      !ReadBoolMember(*integration_object, "deterministic",
                      surface
                          .metaprogramming_macro_host_process_cache_deterministic,
                      error) ||
      !ReadStringMember(
          *integration_object, "replay_key",
          surface.metaprogramming_macro_host_process_cache_replay_key, error) ||
      !ReadStringMember(
          *integration_object, "host_executable_relative_path",
          surface
              .metaprogramming_macro_host_process_cache_host_executable_relative_path,
          error) ||
      !ReadStringMember(
          *integration_object, "cache_root_relative_path",
          surface.metaprogramming_macro_host_process_cache_root_relative_path,
          error) ||
      !ReadStringMember(
          *integration_object, "macro_package_identity",
          surface
              .metaprogramming_macro_host_process_cache_package_identity,
          error) ||
      !ReadStringMember(
          *integration_object, "macro_package_lock_identity",
          surface
              .metaprogramming_macro_host_process_cache_package_lock_identity,
          error) ||
      !ReadStringMember(
          *integration_object, "macro_package_trust_identity",
          surface
              .metaprogramming_macro_host_process_cache_package_trust_identity,
          error) ||
      !ReadStringMember(
          *integration_object, "macro_input_content_identity",
          surface
              .metaprogramming_macro_host_process_cache_input_content_identity,
          error) ||
      !ReadStringMember(
          *integration_object, "macro_output_content_identity",
          surface
              .metaprogramming_macro_host_process_cache_output_content_identity,
          error) ||
      !ReadStringMember(
          *integration_object, "macro_host_identity",
          surface.metaprogramming_macro_host_process_cache_host_identity,
          error) ||
      !ReadStringMember(
          *integration_object, "cache_validation_status",
          surface
              .metaprogramming_macro_host_process_cache_validation_status,
          error) ||
      !ReadStringMember(
          *integration_object, "runtime_consumption_artifact_identity",
          surface
              .metaprogramming_macro_host_process_cache_runtime_consumption_artifact_identity,
          error) ||
      !ReadSizeMember(
          *integration_object, "package_replay_generation",
          surface
              .metaprogramming_macro_host_process_cache_package_replay_generation,
          error)) {
    return false;
  }

  if (contract_id !=
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId) {
    error =
        "unexpected metaprogramming macro host process/cache runtime integration contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId) {
    error =
        "unexpected metaprogramming macro host process/cache runtime integration source contract id in import surface";
    return false;
  }

  surface.metaprogramming_macro_host_process_cache_runtime_integration_present =
      true;
  surface.metaprogramming_macro_host_process_cache_contract_id =
      std::move(contract_id);
  surface.metaprogramming_macro_host_process_cache_source_contract_id =
      std::move(source_contract_id);
  if (!IsReadyObjc3ImportedMetaprogrammingMacroHostProcessCacheRuntimeImportSurface(
          surface)) {
    error =
        "metaprogramming macro host process/cache package replay surface is incomplete";
    return false;
  }
  return true;
}

}  // namespace

bool PopulateImportedMetaprogrammingEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  if (!PopulateImportedMetaprogrammingModuleInterfaceReplayPreservation(
          root, surface, error)) {
    return false;
  }
  return PopulateImportedMetaprogrammingMacroHostProcessCacheRuntimeIntegration(
      root, surface, error);
}

}  // namespace objc3c::pipeline::runtime_import_preservation
