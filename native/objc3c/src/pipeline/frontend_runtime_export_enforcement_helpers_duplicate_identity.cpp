#include "pipeline/frontend_runtime_export_enforcement_helpers_owners.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace objc3c::pipeline::orchestration {
namespace {

template <typename Record, typename KeyBuilder>
Objc3RuntimeExportViolationAccumulator
CountDuplicateRuntimeExportIdentitySitesForRecords(
    const std::vector<Record> &records,
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

}  // namespace

Objc3RuntimeExportViolationAccumulator
CountDuplicateRuntimeExportIdentitySites(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  Objc3RuntimeExportViolationAccumulator duplicate_violations;
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySitesForRecords(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind + "\n" + record.name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySitesForRecords(
      records.categories_lexicographic,
      [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
        return record.record_kind + "\n" + record.class_name + "\n" +
               record.category_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySitesForRecords(
      records.properties_lexicographic,
      [](const Objc3RuntimeMetadataPropertySourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.property_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySitesForRecords(
      records.methods_lexicographic,
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               (record.is_class_method ? "+" : "-") + "\n" + record.selector;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySitesForRecords(
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
  return duplicate_violations;
}

}  // namespace objc3c::pipeline::orchestration
