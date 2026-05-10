#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "artifacts/json/artifact_schema_registry.h"

namespace objc3::artifacts::json {

struct CapabilitySupportSchemaRecord {
  ArtifactSchemaContract contract;
  std::string_view support_data_path;
  std::string_view human_projection_path;
  std::string_view publication_role;
};

struct CapabilitySupportSchemaSummary {
  std::size_t schema_count = 0;
  std::vector<std::string> schema_ids_lexicographic;
  std::vector<std::string> support_data_paths_lexicographic;
  std::vector<std::string> human_projection_paths_lexicographic;
  bool schema_ids_unique = false;
  bool support_data_paths_unique = false;
  bool human_projection_paths_unique = false;
  bool schema_paths_present = false;
};

[[nodiscard]] std::span<const CapabilitySupportSchemaRecord>
CapabilitySupportSchemaRecords();
[[nodiscard]] std::span<const ArtifactSchemaContract>
CapabilitySupportSchemaContracts();
[[nodiscard]] CapabilitySupportSchemaSummary
BuildCapabilitySupportSchemaSummary();
[[nodiscard]] std::optional<CapabilitySupportSchemaRecord>
LookupCapabilitySupportSchemaRecordBySchemaId(std::string_view schema_id);
[[nodiscard]] std::optional<CapabilitySupportSchemaRecord>
LookupCapabilitySupportSchemaRecordBySupportDataPath(std::string_view path);

}  // namespace objc3::artifacts::json
