#pragma once

#include <span>
#include <string_view>

#include "artifacts/json/artifact_schema_registry.h"

namespace objc3::artifacts::json {

struct CapabilitySupportSchemaRecord {
  ArtifactSchemaContract contract;
  std::string_view support_data_path;
  std::string_view human_projection_path;
  std::string_view publication_role;
};

[[nodiscard]] std::span<const CapabilitySupportSchemaRecord>
CapabilitySupportSchemaRecords();
[[nodiscard]] std::span<const ArtifactSchemaContract>
CapabilitySupportSchemaContracts();

}  // namespace objc3::artifacts::json
