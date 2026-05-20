#include "pipeline/frontend_runtime_metadata_source_record_helpers_owners.h"

#include <string>
#include <utility>

#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "support/objc3_property_storage_profile_helpers.h"

namespace objc3c::pipeline::orchestration {

void BuildRuntimeMetadataPropertySynthesisIndex(
    const Objc3Program &program,
    RuntimeMetadataPropertySynthesisIndex &property_synthesis_index) {
  property_synthesis_index.class_implementation_names.reserve(
      program.implementations.size());
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      property_synthesis_index.class_implementation_names.insert(
          implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      property_synthesis_index.implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }
}

void AppendRuntimeMetadataPropertyRecords(
    const std::vector<Objc3PropertyDecl> &properties,
    const std::string &owner_kind,
    const std::string &owner_name,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3RuntimeMetadataSourceRecordSet &records) {
  for (const auto &property : properties) {
    Objc3RuntimeMetadataPropertySourceRecord property_record;
    property_record.owner_kind = owner_kind;
    property_record.owner_name = owner_name;
    property_record.property_name = property.name;
    property_record.type_name = RuntimeMetadataTypeName(property.type);
    property_record.has_getter = property.has_getter;
    property_record.getter_selector = property.getter_selector;
    property_record.has_setter = property.has_setter;
    property_record.setter_selector = property.setter_selector;
    property_record.ivar_binding_symbol = property.ivar_binding_symbol;
    property_record.executable_synthesized_binding_kind =
        property.executable_synthesized_binding_kind;
    property_record.executable_synthesized_binding_symbol =
        property.executable_synthesized_binding_symbol;
    property_record.property_attribute_profile =
        property.property_attribute_profile;
    property_record.property_behavior_declared =
        property.property_behavior_declared;
    property_record.property_behavior_name = property.property_behavior_name;
    property_record.ownership_lifetime_profile =
        property.ownership_lifetime_profile;
    property_record.ownership_runtime_hook_profile =
        property.ownership_runtime_hook_profile;
    property_record.effective_getter_selector =
        property.effective_getter_selector;
    property_record.effective_setter_available =
        property.effective_setter_available;
    property_record.effective_setter_selector =
        property.effective_setter_selector;
    property_record.accessor_ownership_profile =
        property.accessor_ownership_profile;
    objc3c::support::ApplyPropertyOwnershipProfileRetiredRoute(property_record,
                                                           false);
    objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(
        property_record);
    property_record.synthesizes_executable_accessors =
        ShouldSynthesizeExecutablePropertyAccessors(
            owner_kind, owner_name, property.name,
            property_synthesis_index.class_implementation_names,
            property_synthesis_index.implementation_property_keys);
    property_record.getter_storage_runtime_helper_symbol =
        BuildGetterStorageRuntimeHelperSymbol(
            property_record.synthesizes_executable_accessors,
            property_record.ownership_runtime_hook_profile);
    property_record.setter_storage_runtime_helper_symbol =
        BuildSetterStorageRuntimeHelperSymbol(
            property_record.synthesizes_executable_accessors,
            property_record.effective_setter_available,
            property_record.ownership_lifetime_profile,
            property_record.ownership_runtime_hook_profile,
            property_record.accessor_ownership_profile);
    property_record.executable_ivar_layout_symbol =
        property.executable_ivar_layout_symbol;
    property_record.executable_ivar_layout_slot_index =
        property.executable_ivar_layout_slot_index;
    property_record.executable_ivar_layout_size_bytes =
        property.executable_ivar_layout_size_bytes;
    property_record.executable_ivar_layout_alignment_bytes =
        property.executable_ivar_layout_alignment_bytes;
    property_record.executable_ivar_layout_offset_bytes =
        property.executable_ivar_layout_offset_bytes;
    property_record.executable_ivar_layout_padding_bytes =
        property.executable_ivar_layout_padding_bytes;
    property_record.executable_ivar_layout_inherited_slot_count =
        property.executable_ivar_layout_inherited_slot_count;
    property_record.executable_ivar_layout_inherited_size_bytes =
        property.executable_ivar_layout_inherited_size_bytes;
    property_record.executable_ivar_layout_owner_size_bytes =
        property.executable_ivar_layout_owner_size_bytes;
    property_record.executable_ivar_init_order_index =
        property.executable_ivar_init_order_index;
    property_record.executable_ivar_destroy_order_index =
        property.executable_ivar_destroy_order_index;
    property_record.executable_ivar_layout_valid =
        property.executable_ivar_layout_valid;
    property_record.executable_ivar_layout_replay_key =
        property.executable_ivar_layout_replay_key;
    property_record.line = property.line;
    property_record.column = property.column;
    records.properties_lexicographic.push_back(std::move(property_record));

    if (!property.ivar_binding_symbol.empty()) {
      Objc3RuntimeMetadataIvarSourceRecord ivar_record;
      ivar_record.owner_kind = owner_kind;
      ivar_record.owner_name = owner_name;
      ivar_record.property_name = property.name;
      ivar_record.ivar_binding_symbol = property.ivar_binding_symbol;
      ivar_record.executable_synthesized_binding_kind =
          property.executable_synthesized_binding_kind;
      ivar_record.executable_synthesized_binding_symbol =
          property.executable_synthesized_binding_symbol;
      ivar_record.executable_ivar_layout_symbol =
          property.executable_ivar_layout_symbol;
      ivar_record.executable_ivar_layout_slot_index =
          property.executable_ivar_layout_slot_index;
      ivar_record.executable_ivar_layout_size_bytes =
          property.executable_ivar_layout_size_bytes;
      ivar_record.executable_ivar_layout_alignment_bytes =
          property.executable_ivar_layout_alignment_bytes;
      ivar_record.executable_ivar_layout_offset_bytes =
          property.executable_ivar_layout_offset_bytes;
      ivar_record.executable_ivar_layout_padding_bytes =
          property.executable_ivar_layout_padding_bytes;
      ivar_record.executable_ivar_layout_inherited_slot_count =
          property.executable_ivar_layout_inherited_slot_count;
      ivar_record.executable_ivar_layout_inherited_size_bytes =
          property.executable_ivar_layout_inherited_size_bytes;
      ivar_record.executable_ivar_layout_owner_size_bytes =
          property.executable_ivar_layout_owner_size_bytes;
      ivar_record.executable_ivar_init_order_index =
          property.executable_ivar_init_order_index;
      ivar_record.executable_ivar_destroy_order_index =
          property.executable_ivar_destroy_order_index;
      ivar_record.executable_ivar_layout_valid =
          property.executable_ivar_layout_valid;
      ivar_record.executable_ivar_layout_replay_key =
          property.executable_ivar_layout_replay_key;
      ivar_record.line = property.line;
      ivar_record.column = property.column;
      records.ivars_lexicographic.push_back(std::move(ivar_record));
    }
  }
}

void AppendRuntimeMetadataMethodRecords(
    const std::vector<Objc3MethodDecl> &methods,
    const std::string &owner_kind,
    const std::string &owner_name,
    bool direct_members_declared,
    Objc3RuntimeMetadataSourceRecordSet &records) {
  for (const auto &method : methods) {
    Objc3RuntimeMetadataMethodSourceRecord method_record;
    method_record.owner_kind = owner_kind;
    method_record.owner_name = owner_name;
    method_record.selector = method.selector;
    method_record.is_class_method = method.is_class_method;
    method_record.has_body = method.has_body;
    method_record.effective_direct_dispatch =
        method.objc_direct_declared ||
        (direct_members_declared && !method.objc_dynamic_declared);
    method_record.objc_final_declared = method.objc_final_declared;
    method_record.parameter_count = method.params.size();
    method_record.return_type_name = RuntimeMetadataTypeName(method.return_type);
    method_record.line = method.line;
    method_record.column = method.column;
    records.methods_lexicographic.push_back(std::move(method_record));
  }
}

}  // namespace objc3c::pipeline::orchestration
