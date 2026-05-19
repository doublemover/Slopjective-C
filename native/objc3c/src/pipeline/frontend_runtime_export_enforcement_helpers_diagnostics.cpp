#include "pipeline/frontend_runtime_export_enforcement_helpers.h"

#include <algorithm>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "pipeline/frontend_runtime_export_enforcement_helpers_diagnostics_owners.h"

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

std::string RuntimeExportOwnerLabel(const std::string &owner_kind) {
  return owner_kind == "class-interface" || owner_kind == "class-implementation"
             ? "class"
             : "category";
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

void AppendIllegalRuntimePropertyRedeclarationDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  struct RuntimePropertyRedeclarationPair {
    const Objc3RuntimeMetadataPropertySourceRecord *interface_record = nullptr;
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
        pair.implementation_record == nullptr ||
        AreCompatibleRuntimePropertyRedeclarations(
            *pair.interface_record, *pair.implementation_record)) {
      continue;
    }
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, pair.implementation_record->line,
        pair.implementation_record->column, "O3S264",
        "runtime metadata export blocked: illegal runtime redeclaration: "
        "property '" +
            pair.implementation_record->property_name + "' in " +
            RuntimeExportOwnerLabel(pair.implementation_record->owner_kind) +
            " '" + pair.implementation_record->owner_name +
            "' differs between interface and implementation");
  }
}

void AppendIllegalRuntimeMethodRedeclarationDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  struct RuntimeMethodRedeclarationPair {
    const Objc3RuntimeMetadataMethodSourceRecord *interface_record = nullptr;
    const Objc3RuntimeMetadataMethodSourceRecord *implementation_record =
        nullptr;
  };
  std::unordered_map<std::string, RuntimeMethodRedeclarationPair> method_pairs;
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
        pair.implementation_record == nullptr ||
        AreCompatibleRuntimeMethodRedeclarations(
            *pair.interface_record, *pair.implementation_record)) {
      continue;
    }
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, pair.implementation_record->line,
        pair.implementation_record->column, "O3S264",
        "runtime metadata export blocked: illegal runtime redeclaration: " +
            std::string(pair.implementation_record->is_class_method
                            ? "class selector '"
                            : "instance selector '") +
            pair.implementation_record->selector + "' in " +
            RuntimeExportOwnerLabel(pair.implementation_record->owner_kind) +
            " '" + pair.implementation_record->owner_name +
            "' differs between interface and implementation");
  }
}

void AppendIllegalRuntimeExportRedeclarationDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  AppendIllegalRuntimePropertyRedeclarationDiagnostics(records, diagnostics);
  AppendIllegalRuntimeMethodRedeclarationDiagnostics(records, diagnostics);
}

}  // namespace

std::vector<Objc3RuntimeExportBlockingDiagnostic>
BuildRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary) {
  std::vector<Objc3RuntimeExportBlockingDiagnostic> diagnostics;

  if (summary.duplicate_runtime_identity_sites > 0u) {
    AppendDuplicateRuntimeExportBlockingDiagnostics(records, diagnostics);
  }
  AppendIncompleteRuntimeExportBlockingDiagnostics(records, summary,
                                                  diagnostics);
  if (summary.illegal_redeclaration_mix_sites > 0u) {
    AppendIllegalRuntimeExportRedeclarationDiagnostics(records, diagnostics);
  }

  if (summary.duplicate_runtime_identity_sites > 0u && diagnostics.empty()) {
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, summary.first_failure_line, summary.first_failure_column,
        "O3S262",
        "runtime metadata export blocked: duplicate runtime metadata "
        "identities are not exportable");
  }
  if (summary.illegal_redeclaration_mix_sites > 0u && diagnostics.empty()) {
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, summary.first_failure_line, summary.first_failure_column,
        "O3S264",
        "runtime metadata export blocked: illegal runtime metadata "
        "redeclaration mixes are not exportable");
  }

  std::sort(diagnostics.begin(), diagnostics.end(),
            [](const Objc3RuntimeExportBlockingDiagnostic &lhs,
               const Objc3RuntimeExportBlockingDiagnostic &rhs) {
              return std::tie(lhs.line, lhs.column, lhs.code, lhs.message) <
                     std::tie(rhs.line, rhs.column, rhs.code, rhs.message);
            });
  return diagnostics;
}

}  // namespace objc3c::pipeline::orchestration
