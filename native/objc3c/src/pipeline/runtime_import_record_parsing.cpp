#include "pipeline/runtime_import_record_parsing.h"

#include <utility>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3c::pipeline {
namespace {

using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

bool ParseClassRecord(const RuntimeImportJsonValue::Object &object,
                      Objc3RuntimeMetadataClassSourceRecord &record,
                      std::string &error) {
  return ReadStringMember(object, "record_kind", record.record_kind, error) &&
         ReadStringMember(object, "name", record.name, error) &&
         ReadOptionalStringMember(object, "super_name", record.super_name,
                                  error) &&
         ReadBoolMember(object, "has_super", record.has_super, error) &&
         ReadOptionalBoolMember(object, "objc_final_declared",
                                record.objc_final_declared, error) &&
         ReadOptionalBoolMember(object, "objc_sealed_declared",
                                record.objc_sealed_declared, error) &&
         ReadStringArrayMember(object, "adopted_protocols",
                               record.adopted_protocols_lexicographic,
                               error) &&
         ReadSizeMember(object, "property_count", record.property_count,
                        error) &&
         ReadSizeMember(object, "method_count", record.method_count, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseProtocolRecord(const RuntimeImportJsonValue::Object &object,
                         Objc3RuntimeMetadataProtocolSourceRecord &record,
                         std::string &error) {
  return ReadStringMember(object, "name", record.name, error) &&
         ReadStringArrayMember(object, "inherited_protocols",
                               record.inherited_protocols_lexicographic,
                               error) &&
         ReadBoolMember(object, "is_forward_declaration",
                        record.is_forward_declaration, error) &&
         ReadSizeMember(object, "property_count", record.property_count,
                        error) &&
         ReadSizeMember(object, "method_count", record.method_count, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseCategoryRecord(const RuntimeImportJsonValue::Object &object,
                         Objc3RuntimeMetadataCategorySourceRecord &record,
                         std::string &error) {
  return ReadStringMember(object, "record_kind", record.record_kind, error) &&
         ReadStringMember(object, "class_name", record.class_name, error) &&
         ReadStringMember(object, "category_name", record.category_name,
                          error) &&
         ReadStringArrayMember(object, "adopted_protocols",
                               record.adopted_protocols_lexicographic,
                               error) &&
         ReadSizeMember(object, "property_count", record.property_count,
                        error) &&
         ReadSizeMember(object, "method_count", record.method_count, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParsePropertyRecord(const RuntimeImportJsonValue::Object &object,
                         Objc3RuntimeMetadataPropertySourceRecord &record,
                         std::string &error) {
  return ReadStringMember(object, "owner_kind", record.owner_kind, error) &&
         ReadStringMember(object, "owner_name", record.owner_name, error) &&
         ReadStringMember(object, "property_name", record.property_name,
                          error) &&
         ReadStringMember(object, "type", record.type_name, error) &&
         ReadOptionalStringMember(object, "effective_getter_selector",
                                  record.effective_getter_selector, error) &&
         ReadBoolMember(object, "effective_setter_available",
                        record.effective_setter_available, error) &&
         ReadOptionalStringMember(object, "effective_setter_selector",
                                  record.effective_setter_selector, error) &&
         ReadOptionalBoolMember(object, "synthesizes_executable_accessors",
                                record.synthesizes_executable_accessors,
                                error) &&
         ReadOptionalStringMember(
             object, "getter_storage_runtime_helper_symbol",
             record.getter_storage_runtime_helper_symbol, error) &&
         ReadOptionalStringMember(
             object, "setter_storage_runtime_helper_symbol",
             record.setter_storage_runtime_helper_symbol, error) &&
         ReadOptionalStringMember(object, "ivar_binding_symbol",
                                  record.ivar_binding_symbol, error) &&
         ReadOptionalStringMember(
             object, "executable_synthesized_binding_kind",
             record.executable_synthesized_binding_kind, error) &&
         ReadOptionalStringMember(
             object, "executable_synthesized_binding_symbol",
             record.executable_synthesized_binding_symbol, error) &&
         ReadOptionalStringMember(object, "property_attribute_profile",
                                  record.property_attribute_profile, error) &&
         ReadOptionalStringMember(object, "ownership_lifetime_profile",
                                  record.ownership_lifetime_profile, error) &&
         ReadOptionalStringMember(object, "ownership_runtime_hook_profile",
                                  record.ownership_runtime_hook_profile,
                                  error) &&
         ReadOptionalStringMember(object, "accessor_ownership_profile",
                                  record.accessor_ownership_profile, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_symbol",
                                  record.executable_ivar_layout_symbol,
                                  error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_slot_index",
                                record.executable_ivar_layout_slot_index,
                                error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_size_bytes",
                                record.executable_ivar_layout_size_bytes,
                                error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_alignment_bytes",
             record.executable_ivar_layout_alignment_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_offset_bytes",
                                record.executable_ivar_layout_offset_bytes,
                                error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_padding_bytes",
                                record.executable_ivar_layout_padding_bytes,
                                error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_inherited_slot_count",
             record.executable_ivar_layout_inherited_slot_count, error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_inherited_size_bytes",
             record.executable_ivar_layout_inherited_size_bytes, error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_owner_size_bytes",
             record.executable_ivar_layout_owner_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_init_order_index",
                                record.executable_ivar_init_order_index,
                                error) &&
         ReadOptionalSizeMember(object, "executable_ivar_destroy_order_index",
                                record.executable_ivar_destroy_order_index,
                                error) &&
         ReadOptionalBoolMember(object, "executable_ivar_layout_valid",
                                record.executable_ivar_layout_valid, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_replay_key",
                                  record.executable_ivar_layout_replay_key,
                                  error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseMethodRecord(const RuntimeImportJsonValue::Object &object,
                       Objc3RuntimeMetadataMethodSourceRecord &record,
                       std::string &error) {
  return ReadStringMember(object, "owner_kind", record.owner_kind, error) &&
         ReadStringMember(object, "owner_name", record.owner_name, error) &&
         ReadStringMember(object, "selector", record.selector, error) &&
         ReadBoolMember(object, "is_class_method", record.is_class_method,
                        error) &&
         ReadBoolMember(object, "has_body", record.has_body, error) &&
         ReadOptionalBoolMember(object, "effective_direct_dispatch",
                                record.effective_direct_dispatch, error) &&
         ReadOptionalBoolMember(object, "objc_final_declared",
                                record.objc_final_declared, error) &&
         ReadSizeMember(object, "parameter_count", record.parameter_count,
                        error) &&
         ReadStringMember(object, "return_type", record.return_type_name,
                          error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseIvarRecord(const RuntimeImportJsonValue::Object &object,
                     Objc3RuntimeMetadataIvarSourceRecord &record,
                     std::string &error) {
  return ReadStringMember(object, "owner_kind", record.owner_kind, error) &&
         ReadStringMember(object, "owner_name", record.owner_name, error) &&
         ReadStringMember(object, "property_name", record.property_name,
                          error) &&
         ReadOptionalStringMember(object, "ivar_binding_symbol",
                                  record.ivar_binding_symbol, error) &&
         ReadOptionalStringMember(
             object, "executable_synthesized_binding_kind",
             record.executable_synthesized_binding_kind, error) &&
         ReadOptionalStringMember(
             object, "executable_synthesized_binding_symbol",
             record.executable_synthesized_binding_symbol, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_symbol",
                                  record.executable_ivar_layout_symbol,
                                  error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_slot_index",
                                record.executable_ivar_layout_slot_index,
                                error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_size_bytes",
                                record.executable_ivar_layout_size_bytes,
                                error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_alignment_bytes",
             record.executable_ivar_layout_alignment_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_offset_bytes",
                                record.executable_ivar_layout_offset_bytes,
                                error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_padding_bytes",
                                record.executable_ivar_layout_padding_bytes,
                                error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_inherited_slot_count",
             record.executable_ivar_layout_inherited_slot_count, error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_inherited_size_bytes",
             record.executable_ivar_layout_inherited_size_bytes, error) &&
         ReadOptionalSizeMember(
             object, "executable_ivar_layout_owner_size_bytes",
             record.executable_ivar_layout_owner_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_init_order_index",
                                record.executable_ivar_init_order_index,
                                error) &&
         ReadOptionalSizeMember(object, "executable_ivar_destroy_order_index",
                                record.executable_ivar_destroy_order_index,
                                error) &&
         ReadOptionalBoolMember(object, "executable_ivar_layout_valid",
                                record.executable_ivar_layout_valid, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_replay_key",
                                  record.executable_ivar_layout_replay_key,
                                  error) &&
         ReadOptionalStringMember(object, "source_model", record.source_model,
                                  error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

template <typename RecordT>
bool ParseRecordArray(const RuntimeImportJsonValue::Object &root,
                      const std::string &declarations_name,
                      const std::string &record_name,
                      std::vector<RecordT> &records,
                      bool (*parser)(const RuntimeImportJsonValue::Object &,
                                     RecordT &, std::string &),
                      std::string &error) {
  const RuntimeImportJsonValue *declarations_value =
      FindMember(root, declarations_name);
  if (declarations_value == nullptr) {
    error = "missing JSON object member '" + declarations_name + "'";
    return false;
  }
  const RuntimeImportJsonValue::Object *declarations_object =
      AsObject(*declarations_value);
  if (declarations_object == nullptr) {
    error = "JSON member '" + declarations_name + "' must be an object";
    return false;
  }
  const RuntimeImportJsonValue *records_value =
      FindMember(*declarations_object, record_name);
  if (records_value == nullptr) {
    error = "missing JSON array member '" + record_name + "'";
    return false;
  }
  const RuntimeImportJsonValue::Array *records_array =
      AsArray(*records_value);
  if (records_array == nullptr) {
    error = "JSON member '" + record_name + "' must be an array";
    return false;
  }
  records.clear();
  records.reserve(records_array->size());
  for (const RuntimeImportJsonValue &element : *records_array) {
    const RuntimeImportJsonValue::Object *record_object = AsObject(element);
    if (record_object == nullptr) {
      error = "JSON array member '" + record_name + "' must contain objects";
      return false;
    }
    RecordT record;
    if (!parser(*record_object, record, error)) {
      return false;
    }
    records.push_back(std::move(record));
  }
  return true;
}

}  // namespace

bool ParseRuntimeMetadataSourceRecordSet(
    const RuntimeImportJsonValue::Object &root,
    const std::string &declarations_name,
    Objc3RuntimeMetadataSourceRecordSet &record_set,
    std::string &error) {
  if (!ParseRecordArray(root, declarations_name, "classes",
                        record_set.classes_lexicographic, ParseClassRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "protocols",
                        record_set.protocols_lexicographic,
                        ParseProtocolRecord, error) ||
      !ParseRecordArray(root, declarations_name, "categories",
                        record_set.categories_lexicographic,
                        ParseCategoryRecord, error) ||
      !ParseRecordArray(root, declarations_name, "properties",
                        record_set.properties_lexicographic,
                        ParsePropertyRecord, error) ||
      !ParseRecordArray(root, declarations_name, "methods",
                        record_set.methods_lexicographic, ParseMethodRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "ivars",
                        record_set.ivars_lexicographic, ParseIvarRecord,
                        error)) {
    return false;
  }
  record_set.deterministic = true;
  return true;
}

bool ParseSerializedRuntimeMetadataReusePayload(
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
  if (!ParseRuntimeMetadataSourceRecordSet(*payload_object,
                                           "runtime_owned_declarations",
                                           payload_record_set, error)) {
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

}  // namespace objc3c::pipeline
