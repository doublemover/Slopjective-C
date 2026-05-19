#pragma once

#include <span>

#include "artifacts/json/artifact_schema_registry.h"

namespace objc3::artifacts::json {

[[nodiscard]] std::span<const ArtifactSchemaContract>
ArtifactSchemaContractEntries();

}  // namespace objc3::artifacts::json
