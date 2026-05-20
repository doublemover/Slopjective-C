#include "pipeline/frontend_runtime_export_enforcement_helpers_owners.h"

#include <string>
#include <unordered_map>

namespace objc3c::pipeline::orchestration {
namespace {

bool IsInterfaceRuntimePropertyOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" ||
         owner_kind == "category-interface";
}

bool IsImplementationRuntimePropertyOwnerKind(
    const std::string &owner_kind) {
  return owner_kind == "class-implementation" ||
         owner_kind == "category-implementation";
}

bool IsInterfaceRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" ||
         owner_kind == "category-interface";
}

bool IsImplementationRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-implementation" ||
         owner_kind == "category-implementation";
}

bool AreCompatibleRuntimePropertyRedeclarations(
    const Objc3RuntimeMetadataPropertySourceRecord &interface_record,
    const Objc3RuntimeMetadataPropertySourceRecord &implementation_record) {
  return interface_record.type_name == implementation_record.type_name &&
         interface_record.has_getter == implementation_record.has_getter &&
         interface_record.getter_selector ==
             implementation_record.getter_selector &&
         interface_record.has_setter == implementation_record.has_setter &&
         interface_record.setter_selector ==
             implementation_record.setter_selector &&
         interface_record.executable_synthesized_binding_kind ==
             implementation_record.executable_synthesized_binding_kind &&
         interface_record.property_attribute_profile ==
             implementation_record.property_attribute_profile &&
         interface_record.property_behavior_declared ==
             implementation_record.property_behavior_declared &&
         interface_record.property_behavior_name ==
             implementation_record.property_behavior_name &&
         interface_record.ownership_lifetime_profile ==
             implementation_record.ownership_lifetime_profile &&
         interface_record.ownership_runtime_hook_profile ==
             implementation_record.ownership_runtime_hook_profile &&
         interface_record.effective_getter_selector ==
             implementation_record.effective_getter_selector &&
         interface_record.effective_setter_available ==
             implementation_record.effective_setter_available &&
         interface_record.effective_setter_selector ==
             implementation_record.effective_setter_selector &&
         interface_record.accessor_ownership_profile ==
             implementation_record.accessor_ownership_profile &&
         interface_record.executable_ivar_layout_symbol ==
             implementation_record.executable_ivar_layout_symbol &&
         interface_record.executable_ivar_layout_slot_index ==
             implementation_record.executable_ivar_layout_slot_index &&
         interface_record.executable_ivar_layout_size_bytes ==
             implementation_record.executable_ivar_layout_size_bytes &&
         interface_record.executable_ivar_layout_alignment_bytes ==
             implementation_record.executable_ivar_layout_alignment_bytes &&
         interface_record.executable_ivar_layout_offset_bytes ==
             implementation_record.executable_ivar_layout_offset_bytes &&
         interface_record.executable_ivar_layout_padding_bytes ==
             implementation_record.executable_ivar_layout_padding_bytes &&
         interface_record.executable_ivar_layout_inherited_slot_count ==
             implementation_record.executable_ivar_layout_inherited_slot_count &&
         interface_record.executable_ivar_layout_inherited_size_bytes ==
             implementation_record.executable_ivar_layout_inherited_size_bytes &&
         interface_record.executable_ivar_layout_owner_size_bytes ==
             implementation_record.executable_ivar_layout_owner_size_bytes &&
         interface_record.executable_ivar_layout_valid ==
             implementation_record.executable_ivar_layout_valid &&
         interface_record.executable_ivar_layout_replay_key ==
             implementation_record.executable_ivar_layout_replay_key;
}

bool AreCompatibleRuntimeMethodRedeclarations(
    const Objc3RuntimeMetadataMethodSourceRecord &interface_record,
    const Objc3RuntimeMetadataMethodSourceRecord &implementation_record) {
  return interface_record.is_class_method ==
             implementation_record.is_class_method &&
         interface_record.selector == implementation_record.selector &&
         interface_record.effective_direct_dispatch ==
             implementation_record.effective_direct_dispatch &&
         interface_record.objc_final_declared ==
             implementation_record.objc_final_declared &&
         interface_record.parameter_count ==
             implementation_record.parameter_count &&
         interface_record.return_type_name ==
             implementation_record.return_type_name &&
         !interface_record.has_body && implementation_record.has_body;
}

}  // namespace

Objc3RuntimeExportViolationAccumulator
CountIllegalRuntimeExportRedeclarationSites(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  Objc3RuntimeExportViolationAccumulator illegal_redeclaration_violations;
  {
    struct RuntimePropertyRedeclarationPair {
      const Objc3RuntimeMetadataPropertySourceRecord *interface_record =
          nullptr;
      const Objc3RuntimeMetadataPropertySourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimePropertyRedeclarationPair>
        property_pairs;
    property_pairs.reserve(records.properties_lexicographic.size());
    for (const auto &record : records.properties_lexicographic) {
      if (!IsInterfaceRuntimePropertyOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" + record.property_name;
      RuntimePropertyRedeclarationPair &pair = property_pairs[key];
      if (IsInterfaceRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : property_pairs) {
      const RuntimePropertyRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr ||
          pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimePropertyRedeclarations(
              *pair.interface_record, *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  {
    struct RuntimeMethodRedeclarationPair {
      const Objc3RuntimeMetadataMethodSourceRecord *interface_record = nullptr;
      const Objc3RuntimeMetadataMethodSourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimeMethodRedeclarationPair>
        method_pairs;
    method_pairs.reserve(records.methods_lexicographic.size());
    for (const auto &record : records.methods_lexicographic) {
      if (!IsInterfaceRuntimeMethodOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" +
                              (record.is_class_method ? "+" : "-") + "\n" +
                              record.selector;
      RuntimeMethodRedeclarationPair &pair = method_pairs[key];
      if (IsInterfaceRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : method_pairs) {
      const RuntimeMethodRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr ||
          pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimeMethodRedeclarations(
              *pair.interface_record, *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  illegal_redeclaration_violations.count +=
      runtime_export_legality.invalid_protocol_composition_sites +
      runtime_export_legality.property_attribute_invalid_entries +
      runtime_export_legality.property_attribute_contract_violations +
      runtime_export_legality.invalid_type_annotation_sites +
      runtime_export_legality.property_ivar_binding_conflicts;
  return illegal_redeclaration_violations;
}

}  // namespace objc3c::pipeline::orchestration
