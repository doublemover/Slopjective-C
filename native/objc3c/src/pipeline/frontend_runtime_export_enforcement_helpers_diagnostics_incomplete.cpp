#include "pipeline/frontend_runtime_export_enforcement_helpers_diagnostics_owners.h"

#include <string>
#include <unordered_map>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {
namespace {

void AppendClassIncompleteDeclarationDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  std::unordered_map<std::string, Objc3RuntimeExportDiagnosticPairPresence>
      class_presence;
  class_presence.reserve(records.classes_lexicographic.size());
  for (const auto &record : records.classes_lexicographic) {
    Objc3RuntimeExportDiagnosticPairPresence &presence =
        class_presence[record.name];
    CaptureRuntimeExportDiagnosticLocation(presence, record.line,
                                           record.column);
    if (record.record_kind == "interface") {
      ++presence.interface_records;
    } else if (record.record_kind == "implementation") {
      ++presence.implementation_records;
    }
  }

  for (const auto &[name, presence] : class_presence) {
    if (presence.interface_records > 0u &&
        presence.implementation_records == 0u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S260",
          "runtime metadata export blocked: incomplete runtime metadata "
          "declarations are not exportable: interface '" +
              name + "' is missing a matching @implementation");
    } else if (presence.interface_records == 0u &&
               presence.implementation_records > 0u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S260",
          "runtime metadata export blocked: incomplete runtime metadata "
          "declarations are not exportable: implementation '" +
              name + "' is missing a matching @interface");
    }
  }
}

void AppendCategoryIncompleteDeclarationDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  std::unordered_map<std::string, Objc3RuntimeExportDiagnosticPairPresence>
      category_presence;
  category_presence.reserve(records.categories_lexicographic.size());
  for (const auto &record : records.categories_lexicographic) {
    const std::string owner_name =
        BuildCategoryOwnerName(record.class_name, record.category_name);
    Objc3RuntimeExportDiagnosticPairPresence &presence =
        category_presence[owner_name];
    CaptureRuntimeExportDiagnosticLocation(presence, record.line,
                                           record.column);
    if (record.record_kind == "interface") {
      ++presence.interface_records;
    } else if (record.record_kind == "implementation") {
      ++presence.implementation_records;
    }
  }

  for (const auto &[owner_name, presence] : category_presence) {
    if (presence.interface_records > 0u &&
        presence.implementation_records == 0u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S260",
          "runtime metadata export blocked: incomplete runtime metadata "
          "declarations are not exportable: category '" +
              owner_name + "' is missing a matching @implementation");
    } else if (presence.interface_records == 0u &&
               presence.implementation_records > 0u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S260",
          "runtime metadata export blocked: incomplete runtime metadata "
          "declarations are not exportable: category '" +
              owner_name + "' is missing a matching @interface");
    }
  }
}

}  // namespace

void AppendIncompleteRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  if (summary.incomplete_declaration_sites > 0u) {
    AppendClassIncompleteDeclarationDiagnostics(records, diagnostics);
  }
  AppendCategoryIncompleteDeclarationDiagnostics(records, diagnostics);
}

}  // namespace objc3c::pipeline::orchestration
