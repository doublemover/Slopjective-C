#include "pipeline/runtime_import_preservation_owners.h"

#include <string>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedTypeSystemNullabilityContractEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value =
      FindMember(root, "objc_type_system_nullability_contract_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "type-system nullability contract preservation must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadSizeMember(*preservation_object, "canonical_type_count",
                      surface.type_system_nullability_canonical_type_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "object_type_count",
                      surface.type_system_nullability_object_type_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "nullable_entry_count",
                      surface.type_system_nullable_entry_count, error) ||
      !ReadSizeMember(*preservation_object, "nonnull_entry_count",
                      surface.type_system_nonnull_entry_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "implicitly_unwrapped_entry_count",
                      surface.type_system_implicitly_unwrapped_entry_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "null_resettable_entry_count",
                      surface.type_system_null_resettable_entry_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "unspecified_nullability_entry_count",
                      surface.type_system_unspecified_nullability_entry_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "invalid_nullability_entry_count",
                      surface.type_system_invalid_nullability_entry_count,
                      error) ||
      !ReadBoolMember(*preservation_object, "ready",
                      surface.type_system_nullability_contract_ready, error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.type_system_nullability_contract_deterministic,
                      error) ||
      !ReadStringMember(
          *preservation_object, "type_semantic_replay_key",
          surface.type_system_nullability_type_semantic_replay_key, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.type_system_nullability_contract_replay_key,
                        error)) {
    return false;
  }

  if (contract_id != kObjc3TypeSystemNullabilityContractPreservationContractId) {
    error =
        "unexpected type-system nullability contract preservation id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3TypeSystemTypeSemanticModelContractId) {
    error =
        "unexpected type-system nullability contract preservation source contract id";
    return false;
  }
  if (!surface.type_system_nullability_contract_ready ||
      !surface.type_system_nullability_contract_deterministic ||
      surface.type_system_nullability_contract_replay_key.empty() ||
      surface.type_system_nullability_type_semantic_replay_key.empty()) {
    error =
        "type-system nullability contract preservation is not ready for import";
    return false;
  }
  const std::size_t nullability_entry_count =
      surface.type_system_nullable_entry_count +
      surface.type_system_nonnull_entry_count +
      surface.type_system_implicitly_unwrapped_entry_count +
      surface.type_system_null_resettable_entry_count +
      surface.type_system_unspecified_nullability_entry_count;
  if (surface.type_system_nullability_canonical_type_count !=
      nullability_entry_count) {
    error =
        "type-system nullability contract preservation dropped canonical nullability entries";
    return false;
  }

  surface.type_system_nullability_contract_preservation_present = true;
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
