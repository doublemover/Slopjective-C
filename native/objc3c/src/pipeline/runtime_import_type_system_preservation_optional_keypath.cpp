#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <utility>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedTypeSystemOptionalKeypathSurfaceEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *lowering_value =
      FindMember(root, "objc_type_system_optional_keypath_lowering_contract");
  const RuntimeImportJsonValue *runtime_value = FindMember(
      root, "objc_type_system_optional_keypath_runtime_helper_contract");
  if (lowering_value == nullptr && runtime_value == nullptr) {
    return true;
  }
  if (lowering_value == nullptr || runtime_value == nullptr) {
    error =
        "runtime import surface must publish both optional/keypath lowering and runtime helper contracts together";
    return false;
  }

  const RuntimeImportJsonValue::Object *lowering_object =
      AsObject(*lowering_value);
  const RuntimeImportJsonValue::Object *runtime_object =
      AsObject(*runtime_value);
  if (lowering_object == nullptr || runtime_object == nullptr) {
    error =
        "optional/keypath imported contract members must both be JSON objects";
    return false;
  }

  std::string lowering_contract_id;
  std::string lowering_replay_key;
  std::string runtime_contract_id;
  std::string runtime_replay_key;
  if (!ReadStringMember(*lowering_object, "contract_id", lowering_contract_id,
                        error) ||
      !ReadSizeMember(*lowering_object, "optional_send_sites",
                      surface.type_system_optional_send_sites, error) ||
      !ReadSizeMember(*lowering_object, "typed_keypath_literal_sites",
                      surface.type_system_typed_keypath_literal_sites, error) ||
      !ReadSizeMember(*lowering_object, "live_optional_lowering_sites",
                      surface.type_system_live_optional_lowering_sites, error) ||
      !ReadSizeMember(*lowering_object, "live_typed_keypath_artifact_sites",
                      surface.type_system_live_typed_keypath_artifact_sites,
                      error) ||
      !ReadBoolMember(*lowering_object, "ready_for_native_optional_lowering",
                      surface.type_system_ready_for_native_optional_lowering,
                      error) ||
      !ReadStringMember(*lowering_object, "replay_key", lowering_replay_key,
                        error) ||
      !ReadStringMember(*runtime_object, "contract_id", runtime_contract_id,
                        error) ||
      !ReadBoolMember(*runtime_object, "optional_send_runtime_ready",
                      surface.type_system_optional_send_runtime_ready, error) ||
      !ReadBoolMember(*runtime_object, "typed_keypath_descriptor_handles_ready",
                      surface.type_system_typed_keypath_descriptor_handles_ready,
                      error) ||
      !ReadBoolMember(
          *runtime_object, "typed_keypath_runtime_execution_helper_landed",
          surface.type_system_typed_keypath_runtime_execution_helper_landed,
          error) ||
      !ReadStringMember(*runtime_object, "replay_key", runtime_replay_key,
                        error)) {
    return false;
  }

  if (lowering_contract_id !=
      kObjc3TypeSystemOptionalKeypathLoweringContractId) {
    error = "unexpected optional/keypath lowering contract id in import surface";
    return false;
  }
  if (runtime_contract_id !=
      kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId) {
    error =
        "unexpected optional/keypath runtime helper contract id in import surface";
    return false;
  }

  surface.type_system_optional_keypath_lowering_contract_present = true;
  surface.type_system_optional_keypath_lowering_replay_key =
      std::move(lowering_replay_key);
  surface.type_system_optional_keypath_runtime_helper_replay_key =
      std::move(runtime_replay_key);
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
