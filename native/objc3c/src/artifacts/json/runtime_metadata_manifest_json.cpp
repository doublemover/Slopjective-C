#include "artifacts/json/runtime_metadata_manifest_json.h"

#include <ostream>
#include <sstream>

#include "io/json/json_writer.h"

namespace objc3::artifacts::json {
namespace {

using objc3::io::json::JsonObjectWriter;

void WriteArraySeparator(std::ostream &out, bool &first) {
  if (!first) {
    out << ',';
  }
  first = false;
}

template <typename RecordT, typename WriteRecordFn>
std::string RenderRecordArray(const std::vector<RecordT> &records,
                              WriteRecordFn write_record) {
  std::ostringstream out;
  out << '[';
  bool first = true;
  for (const auto &record : records) {
    WriteArraySeparator(out, first);
    write_record(out, record);
  }
  out << ']';
  return out.str();
}

template <typename Predicate>
std::string RenderFilteredClassRecordArray(
    const std::vector<Objc3RuntimeMetadataClassSourceRecord> &records,
    Predicate include_record,
    void (*write_record)(std::ostream &,
                         const Objc3RuntimeMetadataClassSourceRecord &)) {
  std::ostringstream out;
  out << '[';
  bool first = true;
  for (const auto &record : records) {
    if (!include_record(record)) {
      continue;
    }
    WriteArraySeparator(out, first);
    write_record(out, record);
  }
  out << ']';
  return out.str();
}

void WriteLocationFields(JsonObjectWriter &object, unsigned line,
                         unsigned column) {
  object.UnsignedField("line", line);
  object.UnsignedField("column", column);
}

void WriteInterfaceRecord(
    std::ostream &out, const Objc3RuntimeMetadataClassSourceRecord &record) {
  JsonObjectWriter object(out);
  object.StringField("name", record.name);
  object.StringField("super", record.super_name);
  object.StringArrayField("adopted_protocols",
                          record.adopted_protocols_lexicographic);
  object.BoolField("has_super", record.has_super);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  WriteLocationFields(object, record.line, record.column);
  object.End();
}

void WriteImplementationRecord(
    std::ostream &out, const Objc3RuntimeMetadataClassSourceRecord &record) {
  JsonObjectWriter object(out);
  object.StringField("name", record.name);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  WriteLocationFields(object, record.line, record.column);
  object.End();
}

void WriteProtocolRecord(
    std::ostream &out, const Objc3RuntimeMetadataProtocolSourceRecord &record) {
  JsonObjectWriter object(out);
  object.StringField("name", record.name);
  object.BoolField("forward_declaration", record.is_forward_declaration);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  object.StringArrayField("inherited_protocols",
                          record.inherited_protocols_lexicographic);
  WriteLocationFields(object, record.line, record.column);
  object.End();
}

void WriteCategoryRecord(
    std::ostream &out, const Objc3RuntimeMetadataCategorySourceRecord &record) {
  JsonObjectWriter object(out);
  object.StringField("record_kind", record.record_kind);
  object.StringField("class_name", record.class_name);
  object.StringField("category_name", record.category_name);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  object.StringArrayField("adopted_protocols",
                          record.adopted_protocols_lexicographic);
  WriteLocationFields(object, record.line, record.column);
  object.End();
}

void WritePropertyRecord(
    std::ostream &out, const Objc3RuntimeMetadataPropertySourceRecord &record) {
  JsonObjectWriter object(out);
  object.StringField("owner_kind", record.owner_kind);
  object.StringField("owner_name", record.owner_name);
  object.StringField("property_name", record.property_name);
  object.StringField("type", record.type_name);
  object.BoolField("has_getter", record.has_getter);
  object.StringField("getter_selector", record.getter_selector);
  object.BoolField("has_setter", record.has_setter);
  object.StringField("setter_selector", record.setter_selector);
  object.StringField("ivar_binding_symbol", record.ivar_binding_symbol);
  object.StringField("executable_synthesized_binding_kind",
                     record.executable_synthesized_binding_kind);
  object.StringField("executable_synthesized_binding_symbol",
                     record.executable_synthesized_binding_symbol);
  object.StringField("property_attribute_profile",
                     record.property_attribute_profile);
  object.StringField("ownership_lifetime_profile",
                     record.ownership_lifetime_profile);
  object.StringField("ownership_runtime_hook_profile",
                     record.ownership_runtime_hook_profile);
  object.StringField("effective_getter_selector",
                     record.effective_getter_selector);
  object.BoolField("effective_setter_available",
                   record.effective_setter_available);
  object.StringField("effective_setter_selector",
                     record.effective_setter_selector);
  object.StringField("accessor_ownership_profile",
                     record.accessor_ownership_profile);
  object.BoolField("synthesizes_executable_accessors",
                   record.synthesizes_executable_accessors);
  object.StringField("getter_storage_runtime_helper_symbol",
                     record.getter_storage_runtime_helper_symbol);
  object.StringField("setter_storage_runtime_helper_symbol",
                     record.setter_storage_runtime_helper_symbol);
  object.StringField("executable_ivar_layout_symbol",
                     record.executable_ivar_layout_symbol);
  object.SizeField("executable_ivar_layout_slot_index",
                   record.executable_ivar_layout_slot_index);
  object.SizeField("executable_ivar_layout_size_bytes",
                   record.executable_ivar_layout_size_bytes);
  object.SizeField("executable_ivar_layout_alignment_bytes",
                   record.executable_ivar_layout_alignment_bytes);
  object.SizeField("executable_ivar_layout_offset_bytes",
                   record.executable_ivar_layout_offset_bytes);
  object.SizeField("executable_ivar_layout_padding_bytes",
                   record.executable_ivar_layout_padding_bytes);
  object.SizeField("executable_ivar_layout_inherited_slot_count",
                   record.executable_ivar_layout_inherited_slot_count);
  object.SizeField("executable_ivar_layout_inherited_size_bytes",
                   record.executable_ivar_layout_inherited_size_bytes);
  object.SizeField("executable_ivar_layout_owner_size_bytes",
                   record.executable_ivar_layout_owner_size_bytes);
  object.SizeField("executable_ivar_init_order_index",
                   record.executable_ivar_init_order_index);
  object.SizeField("executable_ivar_destroy_order_index",
                   record.executable_ivar_destroy_order_index);
  object.BoolField("executable_ivar_layout_valid",
                   record.executable_ivar_layout_valid);
  object.StringField("executable_ivar_layout_replay_key",
                     record.executable_ivar_layout_replay_key);
  WriteLocationFields(object, record.line, record.column);
  object.End();
}

void WriteMethodRecord(
    std::ostream &out, const Objc3RuntimeMetadataMethodSourceRecord &record) {
  JsonObjectWriter object(out);
  object.StringField("owner_kind", record.owner_kind);
  object.StringField("owner_name", record.owner_name);
  object.StringField("selector", record.selector);
  object.BoolField("is_class_method", record.is_class_method);
  object.BoolField("has_body", record.has_body);
  object.SizeField("parameter_count", record.parameter_count);
  object.StringField("return_type", record.return_type_name);
  WriteLocationFields(object, record.line, record.column);
  object.End();
}

void WriteIvarRecord(std::ostream &out,
                     const Objc3RuntimeMetadataIvarSourceRecord &record) {
  JsonObjectWriter object(out);
  object.StringField("owner_kind", record.owner_kind);
  object.StringField("owner_name", record.owner_name);
  object.StringField("property_name", record.property_name);
  object.StringField("ivar_binding_symbol", record.ivar_binding_symbol);
  object.StringField("executable_synthesized_binding_kind",
                     record.executable_synthesized_binding_kind);
  object.StringField("executable_synthesized_binding_symbol",
                     record.executable_synthesized_binding_symbol);
  object.StringField("executable_ivar_layout_symbol",
                     record.executable_ivar_layout_symbol);
  object.SizeField("executable_ivar_layout_slot_index",
                   record.executable_ivar_layout_slot_index);
  object.SizeField("executable_ivar_layout_size_bytes",
                   record.executable_ivar_layout_size_bytes);
  object.SizeField("executable_ivar_layout_alignment_bytes",
                   record.executable_ivar_layout_alignment_bytes);
  object.SizeField("executable_ivar_layout_offset_bytes",
                   record.executable_ivar_layout_offset_bytes);
  object.SizeField("executable_ivar_layout_padding_bytes",
                   record.executable_ivar_layout_padding_bytes);
  object.SizeField("executable_ivar_layout_inherited_slot_count",
                   record.executable_ivar_layout_inherited_slot_count);
  object.SizeField("executable_ivar_layout_inherited_size_bytes",
                   record.executable_ivar_layout_inherited_size_bytes);
  object.SizeField("executable_ivar_layout_owner_size_bytes",
                   record.executable_ivar_layout_owner_size_bytes);
  object.SizeField("executable_ivar_init_order_index",
                   record.executable_ivar_init_order_index);
  object.SizeField("executable_ivar_destroy_order_index",
                   record.executable_ivar_destroy_order_index);
  object.BoolField("executable_ivar_layout_valid",
                   record.executable_ivar_layout_valid);
  object.StringField("executable_ivar_layout_replay_key",
                     record.executable_ivar_layout_replay_key);
  object.StringField("source_model", record.source_model);
  WriteLocationFields(object, record.line, record.column);
  object.End();
}

}  // namespace

void WriteRuntimeMetadataInterfaceManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderFilteredClassRecordArray(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind == "interface";
      },
      WriteInterfaceRecord);
}

void WriteRuntimeMetadataImplementationManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderFilteredClassRecordArray(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind == "implementation";
      },
      WriteImplementationRecord);
}

void WriteRuntimeMetadataProtocolManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderRecordArray(records.protocols_lexicographic, WriteProtocolRecord);
}

void WriteRuntimeMetadataCategoryManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderRecordArray(records.categories_lexicographic, WriteCategoryRecord);
}

void WriteRuntimeMetadataSourceRecordSetManifestObject(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  JsonObjectWriter object(out);
  object.BoolField("deterministic", records.deterministic);
  object.RawJsonField(
      "properties",
      RenderRecordArray(records.properties_lexicographic, WritePropertyRecord));
  object.RawJsonField("methods", RenderRecordArray(records.methods_lexicographic,
                                                   WriteMethodRecord));
  object.RawJsonField("ivars",
                      RenderRecordArray(records.ivars_lexicographic,
                                        WriteIvarRecord));
  object.End();
}

}  // namespace objc3::artifacts::json
