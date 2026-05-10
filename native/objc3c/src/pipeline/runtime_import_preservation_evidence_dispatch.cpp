#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <utility>

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedDispatchEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value =
      FindMember(root, "objc_dispatch_dispatch_metadata_and_interface_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "dispatch dispatch metadata/interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.dispatch_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.dispatch_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.dispatch_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.dispatch_replay_key, error) ||
      !ReadStringMember(*preservation_object, "lowering_replay_key",
                        surface.dispatch_lowering_replay_key, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_direct_callable_record_count",
                      surface.dispatch_local_direct_callable_record_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_callable_record_count",
                      surface.dispatch_local_final_callable_record_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_container_record_count",
                      surface.dispatch_local_final_container_record_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_sealed_container_record_count",
                      surface.dispatch_local_sealed_container_record_count,
                      error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.dispatch.dispatch.metadata.interface.preservation.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.dispatch.dispatch.control.lowering.contract.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation source contract id in import surface";
    return false;
  }

  surface.dispatch_dispatch_metadata_interface_preservation_present = true;
  surface.dispatch_contract_id = std::move(contract_id);
  surface.dispatch_source_contract_id = std::move(source_contract_id);
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
