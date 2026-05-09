#include "pipeline/frontend_runtime_export_enforcement_helpers.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {
namespace {

struct Objc3RuntimeExportViolationAccumulator {
  std::size_t count = 0;
  unsigned first_line = 1;
  unsigned first_column = 1;
  bool has_location = false;

  void Add(std::size_t increment, unsigned line, unsigned column) {
    if (increment == 0) {
      return;
    }
    if (!has_location) {
      first_line = line;
      first_column = column;
      has_location = true;
    }
    count += increment;
  }

  void Merge(const Objc3RuntimeExportViolationAccumulator &other) {
    if (other.count == 0) {
      return;
    }
    Add(other.count, other.first_line, other.first_column);
  }
};

template <typename Record, typename KeyBuilder>
Objc3RuntimeExportViolationAccumulator
CountDuplicateRuntimeExportIdentitySites(const std::vector<Record> &records,
                                         KeyBuilder build_key) {
  Objc3RuntimeExportViolationAccumulator violations;
  std::unordered_map<std::string, std::size_t> seen;
  seen.reserve(records.size());
  for (const auto &record : records) {
    std::size_t &count = seen[build_key(record)];
    if (count > 0u) {
      violations.Add(1u, record.line, record.column);
    }
    ++count;
  }
  return violations;
}

struct Objc3RuntimeExportPairPresence {
  std::size_t interface_records = 0;
  std::size_t implementation_records = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

struct Objc3RuntimeExportDuplicateSite {
  std::size_t count = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

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

std::vector<Objc3RuntimeExportBlockingDiagnostic>
BuildRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary) {
  std::vector<Objc3RuntimeExportBlockingDiagnostic> diagnostics;
  const auto push_diagnostic =
      [&diagnostics](unsigned line, unsigned column, const std::string &code,
                     const std::string &message) {
        diagnostics.push_back({line, column, code, message});
      };
  const auto pluralize = [](std::size_t count, const std::string &singular,
                            const std::string &plural) {
    return count == 1u ? singular : plural;
  };

  if (summary.duplicate_runtime_identity_sites > 0u) {
    {
      std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
          category_presence;
      category_presence.reserve(records.categories_lexicographic.size());
      for (const auto &record : records.categories_lexicographic) {
        const std::string owner_name =
            BuildCategoryOwnerName(record.class_name, record.category_name);
        Objc3RuntimeExportPairPresence &presence =
            category_presence[owner_name];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        if (record.record_kind == "interface") {
          ++presence.interface_records;
        } else if (record.record_kind == "implementation") {
          ++presence.implementation_records;
        }
      }
      for (const auto &[owner_name, presence] : category_presence) {
        if (presence.interface_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S261",
              "runtime metadata export blocked: category attachment collision: "
              "category '" +
                  owner_name + "' has multiple @interface declarations");
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: category '" +
                  owner_name + "' has multiple @interface attachment "
                                "candidates");
        }
        if (presence.implementation_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S261",
              "runtime metadata export blocked: category attachment collision: "
              "category '" +
                  owner_name + "' has multiple @implementation declarations");
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: category '" +
                  owner_name + "' has multiple @implementation attachment "
                                "candidates");
        }
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
          class_presence;
      class_presence.reserve(records.classes_lexicographic.size());
      for (const auto &record : records.classes_lexicographic) {
        Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        if (record.record_kind == "interface") {
          ++presence.interface_records;
        } else if (record.record_kind == "implementation") {
          ++presence.implementation_records;
        }
      }
      for (const auto &[name, presence] : class_presence) {
        if (presence.interface_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: class '" +
                  name + "' has multiple @interface declarations");
        }
        if (presence.implementation_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: class '" +
                  name + "' has multiple @implementation declarations");
        }
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          property_presence;
      property_presence.reserve(records.properties_lexicographic.size());
      for (const auto &record : records.properties_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" + record.property_name;
        Objc3RuntimeExportDuplicateSite &presence = property_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : property_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string property_name = key.substr(second_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: "
            "property '" +
                property_name + "' in " + owner_kind + " '" + owner_name +
                "' has " + std::to_string(presence.count) + " export " +
                pluralize(presence.count, "record", "records"));
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          method_presence;
      method_presence.reserve(records.methods_lexicographic.size());
      for (const auto &record : records.methods_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" +
                                (record.is_class_method ? "+" : "-") + "\n" +
                                record.selector;
        Objc3RuntimeExportDuplicateSite &presence = method_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : method_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::size_t third_break = key.find('\n', second_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string polarity =
            key.substr(second_break + 1u, third_break - second_break - 1u);
        const std::string selector = key.substr(third_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: " +
                std::string(polarity == "+" ? "class" : "instance") +
                " selector '" + selector + "' in " + owner_kind + " '" +
                owner_name + "' has " + std::to_string(presence.count) +
                " export " + pluralize(presence.count, "record", "records"));
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          ivar_presence;
      ivar_presence.reserve(records.ivars_lexicographic.size());
      for (const auto &record : records.ivars_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" + record.ivar_binding_symbol;
        Objc3RuntimeExportDuplicateSite &presence = ivar_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : ivar_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string ivar_symbol = key.substr(second_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: "
            "ivar '" +
                ivar_symbol + "' in " + owner_kind + " '" + owner_name +
                "' has " + std::to_string(presence.count) + " export " +
                pluralize(presence.count, "record", "records"));
      }
    }
  }

  if (summary.incomplete_declaration_sites > 0u) {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
        class_presence;
    class_presence.reserve(records.classes_lexicographic.size());
    for (const auto &record : records.classes_lexicographic) {
      Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &[name, presence] : class_presence) {
      if (presence.interface_records > 0u &&
          presence.implementation_records == 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: interface '" +
                 name + "' is missing a matching @implementation"});
      } else if (presence.interface_records == 0u &&
                 presence.implementation_records > 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: implementation '" +
                 name + "' is missing a matching @interface"});
      }
    }
  }

  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
        category_presence;
    category_presence.reserve(records.categories_lexicographic.size());
    for (const auto &record : records.categories_lexicographic) {
      const std::string owner_name =
          BuildCategoryOwnerName(record.class_name, record.category_name);
      Objc3RuntimeExportPairPresence &presence = category_presence[owner_name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &[owner_name, presence] : category_presence) {
      if (presence.interface_records > 0u &&
          presence.implementation_records == 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: category '" +
                 owner_name + "' is missing a matching @implementation"});
      } else if (presence.interface_records == 0u &&
                 presence.implementation_records > 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: category '" +
                 owner_name + "' is missing a matching @interface"});
      }
    }
  }

  if (summary.duplicate_runtime_identity_sites > 0u && diagnostics.empty()) {
    push_diagnostic(summary.first_failure_line, summary.first_failure_column,
                    "O3S262",
                    "runtime metadata export blocked: duplicate runtime "
                    "metadata identities are not exportable");
  }

  std::sort(diagnostics.begin(), diagnostics.end(),
            [](const Objc3RuntimeExportBlockingDiagnostic &lhs,
               const Objc3RuntimeExportBlockingDiagnostic &rhs) {
              return std::tie(lhs.line, lhs.column, lhs.code, lhs.message) <
                     std::tie(rhs.line, rhs.column, rhs.code, rhs.message);
            });
  return diagnostics;
}

bool HasRuntimeMetadataSourceRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return !records.classes_lexicographic.empty() ||
         !records.protocols_lexicographic.empty() ||
         !records.categories_lexicographic.empty() ||
         !records.properties_lexicographic.empty() ||
         !records.methods_lexicographic.empty() ||
         !records.ivars_lexicographic.empty();
}

Objc3RuntimeExportEnforcementSummary BuildRuntimeExportEnforcementSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  Objc3RuntimeExportEnforcementSummary summary;
  summary.metadata_completeness_enforced = true;
  summary.duplicate_runtime_identity_suppression_enforced = true;
  summary.illegal_redeclaration_mix_blocking_enforced = true;
  summary.metadata_shape_drift_blocking_enforced = true;
  summary.fail_closed = true;

  Objc3RuntimeExportViolationAccumulator duplicate_violations;
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind + "\n" + record.name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.categories_lexicographic,
      [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
        return record.record_kind + "\n" + record.class_name + "\n" +
               record.category_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.properties_lexicographic,
      [](const Objc3RuntimeMetadataPropertySourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.property_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.methods_lexicographic,
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               (record.is_class_method ? "+" : "-") + "\n" + record.selector;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.ivars_lexicographic,
      [](const Objc3RuntimeMetadataIvarSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.ivar_binding_symbol;
      }));
  {
    std::unordered_map<std::string, std::size_t> seen_protocols;
    seen_protocols.reserve(records.protocols_lexicographic.size());
    for (const auto &record : records.protocols_lexicographic) {
      if (record.is_forward_declaration) {
        continue;
      }
      std::size_t &count = seen_protocols[record.name];
      if (count > 0u) {
        duplicate_violations.Add(1u, record.line, record.column);
      }
      ++count;
    }
  }
  summary.duplicate_runtime_identity_sites = duplicate_violations.count;

  Objc3RuntimeExportViolationAccumulator incomplete_violations;
  // Forward protocol declarations are dependency hints for later complete
  // protocol records or composition spelling; they are not themselves
  // exportable runtime metadata units and must not block the runnable path.
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
        class_presence;
    class_presence.reserve(records.classes_lexicographic.size());
    for (const auto &record : records.classes_lexicographic) {
      Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : class_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u ||
          presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
        category_presence;
    category_presence.reserve(records.categories_lexicographic.size());
    for (const auto &record : records.categories_lexicographic) {
      const std::string key = record.class_name + "\n" + record.category_name;
      Objc3RuntimeExportPairPresence &presence = category_presence[key];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : category_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u ||
          presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  incomplete_violations.count +=
      runtime_export_legality.implementation_resolution_misses +
      runtime_export_legality.method_resolution_misses +
      runtime_export_legality.property_ivar_binding_missing;
  summary.incomplete_declaration_sites = incomplete_violations.count;

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
  summary.illegal_redeclaration_mix_sites =
      illegal_redeclaration_violations.count +
      runtime_export_legality.invalid_protocol_composition_sites +
      runtime_export_legality.property_attribute_invalid_entries +
      runtime_export_legality.property_attribute_contract_violations +
      runtime_export_legality.invalid_type_annotation_sites +
      runtime_export_legality.property_ivar_binding_conflicts;

  summary.metadata_shape_drift_sites =
      (runtime_export_legality.semantic_integration_surface_built ? 0u : 1u) +
      (runtime_export_legality.sema_type_metadata_handoff_deterministic ? 0u
                                                                        : 1u) +
      (runtime_export_legality.typed_sema_surface_ready ? 0u : 1u) +
      (runtime_export_legality.typed_sema_surface_deterministic ? 0u : 1u) +
      (runtime_export_legality.runtime_metadata_source_boundary_ready ? 0u
                                                                     : 1u) +
      (runtime_export_legality.protocol_category_deterministic ? 0u : 1u) +
      (runtime_export_legality.class_protocol_category_linking_deterministic
           ? 0u
           : 1u) +
      (runtime_export_legality.selector_normalization_deterministic ? 0u
                                                                    : 1u) +
      (runtime_export_legality.property_attribute_deterministic ? 0u : 1u) +
      (runtime_export_legality.object_pointer_surface_deterministic ? 0u
                                                                   : 1u) +
      (runtime_export_legality.symbol_graph_scope_resolution_deterministic ? 0u
                                                                          : 1u) +
      (runtime_export_legality.property_synthesis_ivar_binding_deterministic
           ? 0u
           : 1u) +
      (runtime_export_legality.invalid_protocol_composition_sites <=
               runtime_export_legality.protocol_record_count +
                   runtime_export_legality.category_record_count
           ? 0u
           : 1u) +
      (runtime_export_legality.ivar_record_count <=
               runtime_export_legality.property_record_count
           ? 0u
           : 1u);

  summary.ready_for_runtime_export =
      summary.duplicate_runtime_identity_sites == 0u &&
      summary.incomplete_declaration_sites == 0u &&
      summary.illegal_redeclaration_mix_sites == 0u &&
      summary.metadata_shape_drift_sites == 0u;

  if (duplicate_violations.has_location) {
    summary.first_failure_line = duplicate_violations.first_line;
    summary.first_failure_column = duplicate_violations.first_column;
  } else if (incomplete_violations.has_location) {
    summary.first_failure_line = incomplete_violations.first_line;
    summary.first_failure_column = incomplete_violations.first_column;
  } else if (illegal_redeclaration_violations.has_location) {
    summary.first_failure_line = illegal_redeclaration_violations.first_line;
    summary.first_failure_column =
        illegal_redeclaration_violations.first_column;
  }

  if (summary.duplicate_runtime_identity_sites > 0u) {
    summary.failure_reason =
        "duplicate runtime metadata identities are not exportable";
  } else if (summary.incomplete_declaration_sites > 0u) {
    summary.failure_reason =
        "incomplete runtime metadata declarations are not exportable";
  } else if (summary.illegal_redeclaration_mix_sites > 0u) {
    summary.failure_reason =
        "illegal runtime metadata redeclaration mixes are not exportable";
  } else if (summary.metadata_shape_drift_sites > 0u) {
    summary.failure_reason =
        "runtime metadata export shape drift detected before lowering";
  }

  return summary;
}

}  // namespace objc3c::pipeline::orchestration
