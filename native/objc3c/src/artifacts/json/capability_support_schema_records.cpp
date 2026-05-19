#include "artifacts/json/capability_support_schema_records.h"

#include <array>
#include <set>
#include <string>

namespace objc3::artifacts::json {
namespace {

constexpr std::string_view kCapabilitySupportSchemaOwner =
    "native/objc3c/src/artifacts/json/capability_support_schema_records.cpp";

constexpr std::array<CapabilitySupportSchemaRecord, 2>
    kCapabilitySupportSchemaRecords{{
        {{"objc3c-capability-matrix-v1",
          "schema_version",
          "objc3c-capability-matrix-v1",
          "https://objc3c.dev/schemas/objc3c-capability-matrix-v1.schema.json",
          "schemas/objc3c-capability-matrix-v1.schema.json",
          kCapabilitySupportSchemaOwner,
          "capability-support"},
         "docs/support/capability_matrix.json",
         "docs/support/capability_matrix.md",
         "capability-matrix"},
        {{"objc3c-capability-evidence-map-v1",
          "schema_version",
          "objc3c-capability-evidence-map-v1",
          "https://objc3c.dev/schemas/objc3c-capability-evidence-map-v1.schema.json",
          "schemas/objc3c-capability-evidence-map-v1.schema.json",
          kCapabilitySupportSchemaOwner,
          "capability-support"},
         "docs/support/evidence_map.json",
         "docs/support/evidence_map.md",
         "evidence-map"},
    }};

constexpr std::array<ArtifactSchemaContract, 2>
    kCapabilitySupportSchemaContracts{{
        kCapabilitySupportSchemaRecords[0].contract,
        kCapabilitySupportSchemaRecords[1].contract,
    }};

}  // namespace

std::span<const CapabilitySupportSchemaRecord>
CapabilitySupportSchemaRecords() {
  return std::span<const CapabilitySupportSchemaRecord>(
      kCapabilitySupportSchemaRecords.data(),
      kCapabilitySupportSchemaRecords.size());
}

std::span<const ArtifactSchemaContract> CapabilitySupportSchemaContracts() {
  return std::span<const ArtifactSchemaContract>(
      kCapabilitySupportSchemaContracts.data(),
      kCapabilitySupportSchemaContracts.size());
}

CapabilitySupportSchemaSummary BuildCapabilitySupportSchemaSummary() {
  CapabilitySupportSchemaSummary summary;
  std::set<std::string> schema_ids;
  std::set<std::string> support_data_paths;
  std::set<std::string> human_projection_paths;
  summary.schema_paths_present = true;
  for (const CapabilitySupportSchemaRecord &record :
       CapabilitySupportSchemaRecords()) {
    schema_ids.insert(std::string(record.contract.schema_id));
    support_data_paths.insert(std::string(record.support_data_path));
    human_projection_paths.insert(std::string(record.human_projection_path));
    summary.schema_paths_present =
        summary.schema_paths_present && !record.contract.schema_path.empty() &&
        !record.contract.schema_uri.empty();
  }
  summary.schema_count = CapabilitySupportSchemaRecords().size();
  summary.schema_ids_lexicographic.assign(schema_ids.begin(),
                                          schema_ids.end());
  summary.support_data_paths_lexicographic.assign(support_data_paths.begin(),
                                                  support_data_paths.end());
  summary.human_projection_paths_lexicographic.assign(
      human_projection_paths.begin(), human_projection_paths.end());
  summary.schema_ids_unique =
      summary.schema_ids_lexicographic.size() == summary.schema_count;
  summary.support_data_paths_unique =
      summary.support_data_paths_lexicographic.size() == summary.schema_count;
  summary.human_projection_paths_unique =
      summary.human_projection_paths_lexicographic.size() ==
      summary.schema_count;
  return summary;
}

std::optional<CapabilitySupportSchemaRecord>
LookupCapabilitySupportSchemaRecordBySchemaId(std::string_view schema_id) {
  for (const CapabilitySupportSchemaRecord &record :
       CapabilitySupportSchemaRecords()) {
    if (record.contract.schema_id == schema_id) {
      return record;
    }
  }
  return std::nullopt;
}

std::optional<CapabilitySupportSchemaRecord>
LookupCapabilitySupportSchemaRecordBySupportDataPath(std::string_view path) {
  for (const CapabilitySupportSchemaRecord &record :
       CapabilitySupportSchemaRecords()) {
    if (record.support_data_path == path) {
      return record;
    }
  }
  return std::nullopt;
}

}  // namespace objc3::artifacts::json
