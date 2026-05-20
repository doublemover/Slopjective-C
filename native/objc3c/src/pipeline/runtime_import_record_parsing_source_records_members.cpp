#include "pipeline/runtime_import_record_parsing_source_record_owners.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool ParseImportedRuntimePropertySourceRecord(
    const RuntimeImportJsonValue::Object &object,
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
         ReadOptionalBoolMember(object, "property_behavior_declared",
                                record.property_behavior_declared, error) &&
         ReadOptionalStringMember(object, "property_behavior_name",
                                  record.property_behavior_name, error) &&
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

bool ParseImportedRuntimeIvarSourceRecord(
    const RuntimeImportJsonValue::Object &object,
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

}  // namespace objc3c::pipeline::runtime_import_preservation
