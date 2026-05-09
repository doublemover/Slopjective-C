#pragma once

#include <span>
#include <string_view>

#include "artifacts/json/artifact_schema_registry.h"

namespace objc3::artifacts::json {

struct ReleaseReadinessSchemaRecord {
  ArtifactSchemaContract contract;
  std::string_view release_label;
  std::string_view emitted_artifact_suffix;
  std::string_view publication_stage;
};

[[nodiscard]] std::span<const ReleaseReadinessSchemaRecord>
ReleaseReadinessSchemaRecords();
[[nodiscard]] std::span<const ArtifactSchemaContract>
ReleaseReadinessSchemaContracts();

}  // namespace objc3::artifacts::json
