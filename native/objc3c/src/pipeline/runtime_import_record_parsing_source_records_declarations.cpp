#include "pipeline/runtime_import_record_parsing_source_record_owners.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool ParseImportedRuntimeClassSourceRecord(
    const RuntimeImportJsonValue::Object &object,
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

bool ParseImportedRuntimeProtocolSourceRecord(
    const RuntimeImportJsonValue::Object &object,
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

bool ParseImportedRuntimeCategorySourceRecord(
    const RuntimeImportJsonValue::Object &object,
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

bool ParseImportedRuntimeMethodSourceRecord(
    const RuntimeImportJsonValue::Object &object,
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

}  // namespace objc3c::pipeline::runtime_import_preservation
