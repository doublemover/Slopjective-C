#include "pipeline/frontend_runtime_export_enforcement_helpers_diagnostics_owners.h"

#include <string>
#include <unordered_map>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {
namespace {

void AppendCategoryAttachmentCollisionDiagnostics(
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
    if (presence.interface_records > 1u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S261",
          "runtime metadata export blocked: category attachment collision: "
          "category '" +
              owner_name + "' has multiple @interface declarations");
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S263",
          "runtime metadata export blocked: ambiguous runtime metadata graph "
          "resolution: category '" +
              owner_name + "' has multiple @interface attachment candidates");
    }
    if (presence.implementation_records > 1u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S261",
          "runtime metadata export blocked: category attachment collision: "
          "category '" +
              owner_name + "' has multiple @implementation declarations");
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S263",
          "runtime metadata export blocked: ambiguous runtime metadata graph "
          "resolution: category '" +
              owner_name +
              "' has multiple @implementation attachment candidates");
    }
  }
}

void AppendClassIdentityCollisionDiagnostics(
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
    if (presence.interface_records > 1u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S263",
          "runtime metadata export blocked: ambiguous runtime metadata graph "
          "resolution: class '" +
              name + "' has multiple @interface declarations");
    }
    if (presence.implementation_records > 1u) {
      AppendRuntimeExportBlockingDiagnostic(
          diagnostics, presence.line, presence.column, "O3S263",
          "runtime metadata export blocked: ambiguous runtime metadata graph "
          "resolution: class '" +
              name + "' has multiple @implementation declarations");
    }
  }
}

void AppendProtocolIdentityCollisionDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  std::unordered_map<std::string, Objc3RuntimeExportDiagnosticDuplicateSite>
      protocol_presence;
  protocol_presence.reserve(records.protocols_lexicographic.size());
  for (const auto &record : records.protocols_lexicographic) {
    if (record.is_forward_declaration) {
      continue;
    }
    Objc3RuntimeExportDiagnosticDuplicateSite &presence =
        protocol_presence[record.name];
    CaptureRuntimeExportDiagnosticLocation(presence, record.line,
                                           record.column);
    ++presence.count;
  }

  for (const auto &[name, presence] : protocol_presence) {
    if (presence.count <= 1u) {
      continue;
    }
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, presence.line, presence.column, "O3S263",
        "runtime metadata export blocked: ambiguous runtime metadata graph "
        "resolution: protocol '" +
            name + "' has " + std::to_string(presence.count) +
            " concrete export " + Objc3RuntimeExportRecordNoun(presence.count));
  }
}

void AppendPropertyDuplicateDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  std::unordered_map<std::string, Objc3RuntimeExportDiagnosticDuplicateSite>
      property_presence;
  property_presence.reserve(records.properties_lexicographic.size());
  for (const auto &record : records.properties_lexicographic) {
    const std::string key = record.owner_kind + "\n" + record.owner_name +
                            "\n" + record.property_name;
    Objc3RuntimeExportDiagnosticDuplicateSite &presence =
        property_presence[key];
    CaptureRuntimeExportDiagnosticLocation(presence, record.line,
                                           record.column);
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
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, presence.line, presence.column, "O3S262",
        "runtime metadata export blocked: duplicate runtime member: property '" +
            property_name + "' in " + owner_kind + " '" + owner_name +
            "' has " + std::to_string(presence.count) + " export " +
            Objc3RuntimeExportRecordNoun(presence.count));
  }
}

void AppendMethodDuplicateDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  std::unordered_map<std::string, Objc3RuntimeExportDiagnosticDuplicateSite>
      method_presence;
  method_presence.reserve(records.methods_lexicographic.size());
  for (const auto &record : records.methods_lexicographic) {
    const std::string key = record.owner_kind + "\n" + record.owner_name +
                            "\n" + (record.is_class_method ? "+" : "-") +
                            "\n" + record.selector;
    Objc3RuntimeExportDiagnosticDuplicateSite &presence = method_presence[key];
    CaptureRuntimeExportDiagnosticLocation(presence, record.line,
                                           record.column);
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
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, presence.line, presence.column, "O3S262",
        "runtime metadata export blocked: duplicate runtime member: " +
            std::string(polarity == "+" ? "class" : "instance") +
            " selector '" + selector + "' in " + owner_kind + " '" +
            owner_name + "' has " + std::to_string(presence.count) +
            " export " + Objc3RuntimeExportRecordNoun(presence.count));
  }
}

void AppendIvarDuplicateDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  std::unordered_map<std::string, Objc3RuntimeExportDiagnosticDuplicateSite>
      ivar_presence;
  ivar_presence.reserve(records.ivars_lexicographic.size());
  for (const auto &record : records.ivars_lexicographic) {
    const std::string key = record.owner_kind + "\n" + record.owner_name +
                            "\n" + record.ivar_binding_symbol;
    Objc3RuntimeExportDiagnosticDuplicateSite &presence = ivar_presence[key];
    CaptureRuntimeExportDiagnosticLocation(presence, record.line,
                                           record.column);
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
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, presence.line, presence.column, "O3S262",
        "runtime metadata export blocked: duplicate runtime member: ivar '" +
            ivar_symbol + "' in " + owner_kind + " '" + owner_name +
            "' has " + std::to_string(presence.count) + " export " +
            Objc3RuntimeExportRecordNoun(presence.count));
  }
}

}  // namespace

void AppendDuplicateRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics) {
  AppendCategoryAttachmentCollisionDiagnostics(records, diagnostics);
  AppendClassIdentityCollisionDiagnostics(records, diagnostics);
  AppendProtocolIdentityCollisionDiagnostics(records, diagnostics);
  AppendPropertyDuplicateDiagnostics(records, diagnostics);
  AppendMethodDuplicateDiagnostics(records, diagnostics);
  AppendIvarDuplicateDiagnostics(records, diagnostics);
}

}  // namespace objc3c::pipeline::orchestration
