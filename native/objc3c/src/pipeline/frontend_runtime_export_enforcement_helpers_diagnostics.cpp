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

}  // namespace objc3c::pipeline::orchestration
