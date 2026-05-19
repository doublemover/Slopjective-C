#include "artifacts/json/runtime_metadata_manifest_records.h"

namespace objc3::artifacts::json {

void WriteRuntimeMetadataLocationFields(
    objc3::io::json::JsonObjectWriter &object,
    unsigned line,
    unsigned column) {
  object.UnsignedField("line", line);
  object.UnsignedField("column", column);
}

void WriteRuntimeMetadataInterfaceManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataClassSourceRecord &record) {
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("name", record.name);
  object.StringField("super", record.super_name);
  object.StringArrayField("adopted_protocols",
                          record.adopted_protocols_lexicographic);
  object.BoolField("has_super", record.has_super);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  WriteRuntimeMetadataLocationFields(object, record.line, record.column);
  object.End();
}

void WriteRuntimeMetadataImplementationManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataClassSourceRecord &record) {
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("name", record.name);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  WriteRuntimeMetadataLocationFields(object, record.line, record.column);
  object.End();
}

void WriteRuntimeMetadataProtocolManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataProtocolSourceRecord &record) {
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("name", record.name);
  object.BoolField("forward_declaration", record.is_forward_declaration);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  object.StringArrayField("inherited_protocols",
                          record.inherited_protocols_lexicographic);
  WriteRuntimeMetadataLocationFields(object, record.line, record.column);
  object.End();
}

void WriteRuntimeMetadataCategoryManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataCategorySourceRecord &record) {
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("record_kind", record.record_kind);
  object.StringField("class_name", record.class_name);
  object.StringField("category_name", record.category_name);
  object.SizeField("property_count", record.property_count);
  object.SizeField("method_count", record.method_count);
  object.StringArrayField("adopted_protocols",
                          record.adopted_protocols_lexicographic);
  WriteRuntimeMetadataLocationFields(object, record.line, record.column);
  object.End();
}

void WriteRuntimeMetadataPropertyManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataPropertySourceRecord &record) {
  objc3::io::json::JsonObjectWriter object(out);
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
  WriteRuntimeMetadataLocationFields(object, record.line, record.column);
  object.End();
}

void WriteRuntimeMetadataMethodManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataMethodSourceRecord &record) {
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("owner_kind", record.owner_kind);
  object.StringField("owner_name", record.owner_name);
  object.StringField("selector", record.selector);
  object.BoolField("is_class_method", record.is_class_method);
  object.BoolField("has_body", record.has_body);
  object.SizeField("parameter_count", record.parameter_count);
  object.StringField("return_type", record.return_type_name);
  WriteRuntimeMetadataLocationFields(object, record.line, record.column);
  object.End();
}

void WriteRuntimeMetadataIvarManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataIvarSourceRecord &record) {
  objc3::io::json::JsonObjectWriter object(out);
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
  WriteRuntimeMetadataLocationFields(object, record.line, record.column);
  object.End();
}

}  // namespace objc3::artifacts::json
