#include "artifacts/json/artifact_schema_registry.h"

#include <array>
#include <vector>

namespace objc3::artifacts::json {
namespace {

constexpr std::array<ArtifactSchemaContract, 6> kSchemas{{
    {"objc3c-public-command-contract-v1", "schemas/objc3c-public-command-contract-v1.schema.json"},
    {"objc3c-validation-acceptance-artifact-index-v1",
     "schemas/objc3c-validation-acceptance-artifact-index-v1.schema.json"},
    {"objc3c-public-conformance-summary-v1", "schemas/objc3c-public-conformance-summary-v1.schema.json"},
    {"objc3c-public-conformance-scorecard-v1", "schemas/objc3c-public-conformance-scorecard-v1.schema.json"},
    {"objc3c-runtime-performance-telemetry-v1", "schemas/objc3c-runtime-performance-telemetry-v1.schema.json"},
    {"objc3-conformance-dashboard-status/v1", "schemas/objc3-conformance-dashboard-status-v1.schema.json"},
}};

}  // namespace

std::vector<ArtifactSchemaContract> ListArtifactSchemaContracts() {
  return std::vector<ArtifactSchemaContract>(kSchemas.begin(), kSchemas.end());
}

std::optional<ArtifactSchemaContract> LookupArtifactSchemaContract(std::string_view schema_id) {
  for (const ArtifactSchemaContract &contract : kSchemas) {
    if (contract.schema_id == schema_id) {
      return contract;
    }
  }
  return std::nullopt;
}

std::optional<std::string> LookupArtifactSchemaPath(std::string_view schema_id) {
  const std::optional<ArtifactSchemaContract> contract = LookupArtifactSchemaContract(schema_id);
  if (contract.has_value()) {
    return std::string(contract->schema_path);
  }
  return std::nullopt;
}

}  // namespace objc3::artifacts::json
