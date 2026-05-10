#include "pipeline/runtime_import_preservation_owners.h"

#include <string>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedTypeSystemGenericContractEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value =
      FindMember(root, "objc_type_system_generic_contract_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error = "type-system generic contract preservation must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadSizeMember(*preservation_object, "generic_interface_count",
                      surface.type_system_generic_interface_count, error) ||
      !ReadSizeMember(*preservation_object, "generic_parameter_count",
                      surface.type_system_generic_parameter_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "generic_variance_annotation_count",
                      surface.type_system_generic_variance_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "generic_argument_reference_count",
                      surface.type_system_generic_argument_reference_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "protocol_qualified_generic_argument_count",
                      surface
                          .type_system_protocol_qualified_generic_argument_count,
                      error) ||
      !ReadBoolMember(*preservation_object, "ready",
                      surface.type_system_generic_contract_ready, error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.type_system_generic_contract_deterministic,
                      error) ||
      !ReadStringMember(*preservation_object, "type_semantic_replay_key",
                        surface.type_system_generic_type_semantic_replay_key,
                        error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.type_system_generic_contract_replay_key,
                        error)) {
    return false;
  }

  if (contract_id != kObjc3TypeSystemGenericContractPreservationContractId) {
    error =
        "unexpected type-system generic contract preservation id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3TypeSystemTypeSemanticModelContractId) {
    error =
        "unexpected type-system generic contract preservation source contract id";
    return false;
  }
  if (!surface.type_system_generic_contract_ready ||
      !surface.type_system_generic_contract_deterministic ||
      surface.type_system_generic_contract_replay_key.empty() ||
      surface.type_system_generic_type_semantic_replay_key.empty()) {
    error =
        "type-system generic contract preservation is not ready for import";
    return false;
  }
  if (surface.type_system_generic_variance_annotation_count <
      surface.type_system_generic_parameter_count) {
    error =
        "type-system generic contract preservation dropped variance annotations";
    return false;
  }

  surface.type_system_generic_contract_preservation_present = true;
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
