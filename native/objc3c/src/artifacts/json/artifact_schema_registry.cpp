#include "artifacts/json/artifact_schema_registry.h"

#include <array>
#include <utility>

namespace objc3::artifacts::json {

std::optional<std::string> LookupArtifactSchemaPath(std::string_view schema_id) {
  static constexpr std::array<std::pair<std::string_view, std::string_view>, 4> kSchemas{{
      {"objc3c-public-command-contract-v1", "schemas/objc3c-public-command-contract-v1.schema.json"},
      {"objc3c-validation-acceptance-artifact-index-v1",
       "schemas/objc3c-validation-acceptance-artifact-index-v1.schema.json"},
      {"objc3c-public-conformance-summary-v1", "schemas/objc3c-public-conformance-summary-v1.schema.json"},
      {"objc3c-runtime-performance-telemetry-v1", "schemas/objc3c-runtime-performance-telemetry-v1.schema.json"},
  }};
  for (const auto &[id, path] : kSchemas) {
    if (id == schema_id) {
      return std::string(path);
    }
  }
  return std::nullopt;
}

}  // namespace objc3::artifacts::json
