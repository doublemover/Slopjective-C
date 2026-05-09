#include "pipeline/runtime_import_type_system_preservation.h"

#include <utility>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_runtime_import_surface.h"

namespace objc3c::pipeline {

bool PopulateImportedTypeSystemOptionalKeypathSurface(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *lowering_value =
      FindMember(root, "objc_type_system_optional_keypath_lowering_contract");
  const RuntimeImportJsonValue *runtime_value =
      FindMember(root, "objc_type_system_optional_keypath_runtime_helper_contract");
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
      !ReadBoolMember(*runtime_object,
                      "typed_keypath_runtime_execution_helper_landed",
                      surface.type_system_typed_keypath_runtime_execution_helper_landed,
                      error) ||
      !ReadStringMember(*runtime_object, "replay_key", runtime_replay_key,
                        error)) {
    return false;
  }

  if (lowering_contract_id != kObjc3TypeSystemOptionalKeypathLoweringContractId) {
    error = "unexpected optional/keypath lowering contract id in import surface";
    return false;
  }
  if (runtime_contract_id != kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId) {
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

bool PopulateImportedTypeSystemGenericContractPreservation(
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

bool PopulateImportedTypeSystemNullabilityContractPreservation(
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

bool PopulateImportedTypeSystemProtocolContractPreservation(
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

}  // namespace objc3c::pipeline
