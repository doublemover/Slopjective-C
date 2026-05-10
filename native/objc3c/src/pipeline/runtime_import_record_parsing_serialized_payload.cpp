#include "pipeline/runtime_import_preservation_owners.h"

#include <utility>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3c::pipeline::runtime_import_preservation {

using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

bool ParseSerializedRuntimeMetadataReusePayloadContents(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *payload_value =
      FindMember(root,
                 kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName);
  if (payload_value == nullptr) {
    surface.reused_module_names_lexicographic = {
        surface.frontend_closure_summary.module_name};
    surface.uses_serialized_runtime_metadata_payload = false;
    return true;
  }
  const RuntimeImportJsonValue::Object *payload_object =
      AsObject(*payload_value);
  if (payload_object == nullptr) {
    error = "serialized runtime metadata reuse payload must be an object";
    return false;
  }

  std::string contract_id;
  std::string module_name;
  std::string replay_key;
  std::vector<std::string> reused_module_names;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t metadata_reference_count = 0;
  bool ready = false;
  if (!ReadStringMember(*payload_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*payload_object, "module_name", module_name, error) ||
      !ReadStringArrayMember(*payload_object,
                             "reused_module_names_lexicographic",
                             reused_module_names, error) ||
      !ReadSizeMember(*payload_object, "runtime_owned_declaration_count",
                      runtime_owned_declaration_count, error) ||
      !ReadSizeMember(*payload_object, "metadata_reference_count",
                      metadata_reference_count, error) ||
      !ReadBoolMember(*payload_object, "ready", ready, error) ||
      !ReadStringMember(*payload_object, "replay_key", replay_key, error)) {
    return false;
  }
  if (contract_id != kObjc3SerializedRuntimeMetadataArtifactReuseContractId) {
    error = "unexpected serialized runtime metadata reuse payload contract id";
    return false;
  }
  if (!ready) {
    error = "serialized runtime metadata reuse payload is not ready";
    return false;
  }
  if (reused_module_names.empty()) {
    error = "serialized runtime metadata reuse payload must list reused modules";
    return false;
  }

  Objc3RuntimeMetadataSourceRecordSet payload_record_set;
  if (!ParseRuntimeMetadataSourceRecordSetContents(
          *payload_object, "runtime_owned_declarations", payload_record_set,
          error)) {
    return false;
  }
  const RuntimeImportJsonValue *references_value =
      FindMember(*payload_object, "metadata_references");
  if (references_value == nullptr) {
    error =
        "serialized runtime metadata reuse payload is missing metadata_references";
    return false;
  }
  const RuntimeImportJsonValue::Array *references_array =
      AsArray(*references_value);
  if (references_array == nullptr) {
    error =
        "serialized runtime metadata reuse payload metadata_references must be an array";
    return false;
  }
  if (CountRuntimeMetadataSourceRecordSetDeclarations(payload_record_set) !=
      runtime_owned_declaration_count) {
    error =
        "serialized runtime metadata reuse payload declaration count does not match payload inventory";
    return false;
  }
  if (references_array->size() != metadata_reference_count ||
      CountRuntimeMetadataSourceRecordSetReferences(payload_record_set) !=
          metadata_reference_count) {
    error =
        "serialized runtime metadata reuse payload reference count does not match payload inventory";
    return false;
  }

  surface.runtime_metadata_source_records = std::move(payload_record_set);
  surface.reused_module_names_lexicographic = std::move(reused_module_names);
  surface.uses_serialized_runtime_metadata_payload = true;
  (void)module_name;
  (void)replay_key;
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
