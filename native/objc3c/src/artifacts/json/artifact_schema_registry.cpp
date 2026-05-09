#include "artifacts/json/artifact_schema_registry.h"

#include <array>
#include <vector>

namespace objc3::artifacts::json {
namespace {

// Registry-owned artifact schemas keep payload identity separate from file path
// so contract_id and schema_id based artifacts can use the same lookup surface.
constexpr std::array<ArtifactSchemaContract, 6> kSchemas{{
    {"objc3c-public-command-contract-v1",
     "contract_id",
     "objc3c-public-command-contract-v1",
     "https://slopjective-c.dev/schemas/objc3c-public-command-contract-v1.schema.json",
     "schemas/objc3c-public-command-contract-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_registry.cpp",
     "public-command-contract"},
    {"objc3c-validation-acceptance-artifact-index-v1",
     "contract_id",
     "objc3c.validation.acceptance.artifact.index.v1",
     "https://slopjective-c.dev/schemas/objc3c-validation-acceptance-artifact-index-v1.schema.json",
     "schemas/objc3c-validation-acceptance-artifact-index-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_registry.cpp",
     "validation-acceptance"},
    {"objc3c-public-conformance-summary-v1",
     "contract_id",
     "objc3c.public_conformance_reporting.summary.v1",
     "https://schemas.slopjective.local/objc3c-public-conformance-summary-v1.schema.json",
     "schemas/objc3c-public-conformance-summary-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_registry.cpp",
     "public-conformance"},
    {"objc3c-public-conformance-scorecard-v1",
     "contract_id",
     "objc3c.public_conformance_reporting.scorecard.summary.v1",
     "https://schemas.slopjective.local/objc3c-public-conformance-scorecard-v1.schema.json",
     "schemas/objc3c-public-conformance-scorecard-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_registry.cpp",
     "public-conformance"},
    {"objc3c-runtime-performance-telemetry-v1",
     "contract_id",
     "objc3c.runtime.performance.telemetry.v1",
     "https://objc3c.dev/schemas/objc3c-runtime-performance-telemetry-v1.schema.json",
     "schemas/objc3c-runtime-performance-telemetry-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_registry.cpp",
     "runtime-performance"},
    {"objc3-conformance-dashboard-status/v1",
     "schema_id",
     "objc3-conformance-dashboard-status/v1",
     "https://schemas.slopjective.local/objc3-conformance-dashboard-status-v1.schema.json",
     "schemas/objc3-conformance-dashboard-status-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_registry.cpp",
     "conformance-dashboard"},
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

std::optional<ArtifactSchemaContract> LookupArtifactSchemaContractByPayloadId(
    std::string_view payload_id) {
  for (const ArtifactSchemaContract &contract : kSchemas) {
    if (contract.payload_id_value == payload_id) {
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
