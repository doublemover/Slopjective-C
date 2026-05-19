#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <vector>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedTypeSystemProtocolContractEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value =
      FindMember(root, "objc_type_system_protocol_contract_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error = "type-system protocol contract preservation must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadSizeMember(*preservation_object, "protocol_decl_count",
                      surface.type_system_protocol_decl_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "protocol_forward_declaration_count",
                      surface.type_system_protocol_forward_declaration_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "protocol_inheritance_edge_count",
                      surface.type_system_protocol_inheritance_edge_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_required_method_count",
                      surface.type_system_protocol_required_method_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_optional_method_count",
                      surface.type_system_protocol_optional_method_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_required_property_count",
                      surface.type_system_protocol_required_property_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_optional_property_count",
                      surface.type_system_protocol_optional_property_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "class_protocol_adoption_count",
                      surface.type_system_class_protocol_adoption_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "category_protocol_adoption_count",
                      surface.type_system_category_protocol_adoption_count,
                      error) ||
      !ReadBoolMember(*preservation_object, "ready",
                      surface.type_system_protocol_contract_ready, error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.type_system_protocol_contract_deterministic,
                      error) ||
      !ReadStringMember(
          *preservation_object, "type_semantic_replay_key",
          surface.type_system_protocol_type_semantic_replay_key, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.type_system_protocol_contract_replay_key,
                        error)) {
    return false;
  }

  if (contract_id != kObjc3TypeSystemProtocolContractPreservationContractId) {
    error =
        "unexpected type-system protocol contract preservation id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3TypeSystemTypeSemanticModelContractId) {
    error =
        "unexpected type-system protocol contract preservation source contract id";
    return false;
  }
  if (!surface.type_system_protocol_contract_ready ||
      !surface.type_system_protocol_contract_deterministic ||
      surface.type_system_protocol_contract_replay_key.empty() ||
      surface.type_system_protocol_type_semantic_replay_key.empty()) {
    error =
        "type-system protocol contract preservation is not ready for import";
    return false;
  }
  if (surface.type_system_protocol_forward_declaration_count >
      surface.type_system_protocol_decl_count) {
    error =
        "type-system protocol contract preservation has impossible forward declaration count";
    return false;
  }

  const RuntimeImportJsonValue *protocols_value =
      FindMember(*preservation_object, "protocol_contract_protocols");
  if (protocols_value == nullptr) {
    error = "type-system protocol contract preservation is missing protocols";
    return false;
  }
  const RuntimeImportJsonValue::Array *protocols_array =
      AsArray(*protocols_value);
  if (protocols_array == nullptr) {
    error =
        "type-system protocol contract preservation protocols must be an array";
    return false;
  }
  if (protocols_array->size() != surface.type_system_protocol_decl_count) {
    error =
        "type-system protocol contract preservation dropped protocol records";
    return false;
  }

  std::size_t inherited_edge_count = 0;
  std::size_t required_method_count = 0;
  std::size_t optional_method_count = 0;
  std::size_t required_property_count = 0;
  std::size_t optional_property_count = 0;
  std::size_t forward_declaration_count = 0;
  for (const RuntimeImportJsonValue &protocol_value : *protocols_array) {
    const RuntimeImportJsonValue::Object *protocol_object =
        AsObject(protocol_value);
    if (protocol_object == nullptr) {
      error =
          "type-system protocol contract preservation protocols must contain objects";
      return false;
    }
    std::string protocol_name;
    bool is_forward_declaration = false;
    std::size_t required_methods = 0;
    std::size_t optional_methods = 0;
    std::size_t required_properties = 0;
    std::size_t optional_properties = 0;
    std::vector<std::string> inherited_protocols;
    if (!ReadStringMember(*protocol_object, "name", protocol_name, error) ||
        !ReadBoolMember(*protocol_object, "is_forward_declaration",
                        is_forward_declaration, error) ||
        !ReadStringArrayMember(*protocol_object,
                               "inherited_protocols_lexicographic",
                               inherited_protocols, error) ||
        !ReadSizeMember(*protocol_object, "required_method_count",
                        required_methods, error) ||
        !ReadSizeMember(*protocol_object, "optional_method_count",
                        optional_methods, error) ||
        !ReadSizeMember(*protocol_object, "required_property_count",
                        required_properties, error) ||
        !ReadSizeMember(*protocol_object, "optional_property_count",
                        optional_properties, error)) {
      return false;
    }
    if (protocol_name.empty()) {
      error =
          "type-system protocol contract preservation contains an unnamed protocol";
      return false;
    }
    if (is_forward_declaration) {
      ++forward_declaration_count;
    }
    inherited_edge_count += inherited_protocols.size();
    required_method_count += required_methods;
    optional_method_count += optional_methods;
    required_property_count += required_properties;
    optional_property_count += optional_properties;
  }
  if (forward_declaration_count !=
          surface.type_system_protocol_forward_declaration_count ||
      inherited_edge_count !=
          surface.type_system_protocol_inheritance_edge_count ||
      required_method_count !=
          surface.type_system_protocol_required_method_count ||
      optional_method_count !=
          surface.type_system_protocol_optional_method_count ||
      required_property_count !=
          surface.type_system_protocol_required_property_count ||
      optional_property_count !=
          surface.type_system_protocol_optional_property_count) {
    error =
        "type-system protocol contract preservation dropped requirement or inheritance entries";
    return false;
  }

  surface.type_system_protocol_contract_preservation_present = true;
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
