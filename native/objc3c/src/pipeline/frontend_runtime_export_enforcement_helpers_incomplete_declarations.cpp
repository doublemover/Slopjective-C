#include "pipeline/frontend_runtime_export_enforcement_helpers_owners.h"

#include <cstddef>
#include <string>
#include <unordered_map>

namespace objc3c::pipeline::orchestration {
namespace {

struct Objc3RuntimeExportPairPresence {
  std::size_t interface_records = 0;
  std::size_t implementation_records = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

}  // namespace

Objc3RuntimeExportViolationAccumulator
CountIncompleteRuntimeExportDeclarationSites(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
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
  return incomplete_violations;
}

}  // namespace objc3c::pipeline::orchestration
