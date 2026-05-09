#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "pipeline/frontend_metadata_handoff_ordering.h"
#include "pipeline/objc3_frontend_types.h"
#include "support/objc3_property_storage_profile_helpers.h"

namespace objc3c::pipeline::orchestration {

inline Objc3RuntimeMetadataSourceRecordSet BuildRuntimeMetadataSourceRecordSet(
    const Objc3Program &program) {
  Objc3RuntimeMetadataSourceRecordSet records;
  std::unordered_set<std::string> class_implementation_names;
  class_implementation_names.reserve(program.implementations.size());
  std::unordered_set<std::string> implementation_property_keys;
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      class_implementation_names.insert(implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }

  const auto apply_arc_property_interaction_metadata =
      [](Objc3RuntimeMetadataPropertySourceRecord &property_record) {
        objc3c::support::ApplyPropertyOwnershipProfileFallback(
            property_record, false);
        objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(
            property_record);
      };

  const auto append_property_records =
      [&records, &apply_arc_property_interaction_metadata,
       &class_implementation_names, &implementation_property_keys](
          const auto &properties, const std::string &owner_kind,
          const std::string &owner_name) {
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
          apply_arc_property_interaction_metadata(property_record);
          property_record.synthesizes_executable_accessors =
              ShouldSynthesizeExecutablePropertyAccessors(
                  owner_kind, owner_name, property.name,
                  class_implementation_names, implementation_property_keys);
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
      };

  const auto append_method_records =
      [&records](const auto &methods, const std::string &owner_kind,
                 const std::string &owner_name,
                 bool direct_members_declared = false) {
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
      };

  struct Objc3ClassDispatchProfile {
    bool objc_direct_members_declared = false;
    bool objc_final_declared = false;
    bool objc_sealed_declared = false;
  };
  std::unordered_map<std::string, Objc3ClassDispatchProfile> class_dispatch_profiles;
  class_dispatch_profiles.reserve(program.interfaces.size());
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      continue;
    }
    class_dispatch_profiles[interface_decl.name] = Objc3ClassDispatchProfile{
        interface_decl.objc_direct_members_declared,
        interface_decl.objc_final_declared,
        interface_decl.objc_sealed_declared};
  }

  for (const auto &protocol : program.protocols) {
    Objc3RuntimeMetadataProtocolSourceRecord record;
    record.name = protocol.name;
    record.inherited_protocols_lexicographic = protocol.inherited_protocols_lexicographic;
    record.is_forward_declaration = protocol.is_forward_declaration;
    record.property_count = protocol.properties.size();
    record.method_count = protocol.methods.size();
    record.line = protocol.line;
    record.column = protocol.column;
    records.protocols_lexicographic.push_back(record);
    append_property_records(protocol.properties, "protocol", protocol.name);
    append_method_records(protocol.methods, "protocol", protocol.name);
  }

  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "interface";
      record.class_name = interface_decl.name;
      record.category_name = interface_decl.category_name;
      record.adopted_protocols_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      record.property_count = interface_decl.properties.size();
      record.method_count = interface_decl.methods.size();
      record.line = interface_decl.line;
      record.column = interface_decl.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(interface_decl.name, interface_decl.category_name);
      append_property_records(interface_decl.properties, "category-interface", owner_name);
      append_method_records(interface_decl.methods, "category-interface", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    record.record_kind = "interface";
    record.name = interface_decl.name;
    record.super_name = interface_decl.super_name;
    record.adopted_protocols_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    record.has_super = !interface_decl.super_name.empty();
    record.objc_final_declared = interface_decl.objc_final_declared;
    record.objc_sealed_declared = interface_decl.objc_sealed_declared;
    record.property_count = interface_decl.properties.size();
    record.method_count = interface_decl.methods.size();
    record.line = interface_decl.line;
    record.column = interface_decl.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(interface_decl.properties, "class-interface", interface_decl.name);
    append_method_records(interface_decl.methods, "class-interface",
                          interface_decl.name,
                          interface_decl.objc_direct_members_declared);
  }

  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "implementation";
      record.class_name = implementation.name;
      record.category_name = implementation.category_name;
      record.property_count = implementation.properties.size();
      record.method_count = implementation.methods.size();
      record.line = implementation.line;
      record.column = implementation.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(implementation.name, implementation.category_name);
      append_property_records(implementation.properties, "category-implementation", owner_name);
      append_method_records(implementation.methods, "category-implementation", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    const auto profile_it = class_dispatch_profiles.find(implementation.name);
    record.record_kind = "implementation";
    record.name = implementation.name;
    if (profile_it != class_dispatch_profiles.end()) {
      record.objc_final_declared = profile_it->second.objc_final_declared;
      record.objc_sealed_declared = profile_it->second.objc_sealed_declared;
    }
    record.property_count = implementation.properties.size();
    record.method_count = implementation.methods.size();
    record.line = implementation.line;
    record.column = implementation.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(implementation.properties, "class-implementation", implementation.name);
    append_method_records(
        implementation.methods, "class-implementation", implementation.name,
        profile_it != class_dispatch_profiles.end() &&
            profile_it->second.objc_direct_members_declared);
  }

  std::sort(records.classes_lexicographic.begin(),
            records.classes_lexicographic.end(),
            IsClassSourceRecordLess);
  std::sort(records.protocols_lexicographic.begin(),
            records.protocols_lexicographic.end(),
            IsProtocolSourceRecordLess);
  std::sort(records.categories_lexicographic.begin(),
            records.categories_lexicographic.end(),
            IsCategorySourceRecordLess);
  std::sort(records.properties_lexicographic.begin(),
            records.properties_lexicographic.end(),
            IsPropertySourceRecordLess);
  std::sort(records.methods_lexicographic.begin(),
            records.methods_lexicographic.end(),
            IsMethodSourceRecordLess);
  std::sort(records.ivars_lexicographic.begin(),
            records.ivars_lexicographic.end(),
            IsIvarSourceRecordLess);

  records.deterministic = std::is_sorted(records.classes_lexicographic.begin(),
                                         records.classes_lexicographic.end(),
                                         IsClassSourceRecordLess) &&
                          std::is_sorted(records.protocols_lexicographic.begin(),
                                         records.protocols_lexicographic.end(),
                                         IsProtocolSourceRecordLess) &&
                          std::is_sorted(records.categories_lexicographic.begin(),
                                         records.categories_lexicographic.end(),
                                         IsCategorySourceRecordLess) &&
                          std::is_sorted(records.properties_lexicographic.begin(),
                                         records.properties_lexicographic.end(),
                                         IsPropertySourceRecordLess) &&
                          std::is_sorted(records.methods_lexicographic.begin(),
                                         records.methods_lexicographic.end(),
                                         IsMethodSourceRecordLess) &&
                          std::is_sorted(records.ivars_lexicographic.begin(),
                                         records.ivars_lexicographic.end(),
                                         IsIvarSourceRecordLess);
  return records;
}

}  // namespace objc3c::pipeline::orchestration
