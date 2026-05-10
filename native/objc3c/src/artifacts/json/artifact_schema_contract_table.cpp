#include "artifacts/json/artifact_schema_contract_table.h"

#include "artifacts/json/release_readiness_schema_records.h"

#include <array>
#include <vector>

namespace objc3::artifacts::json {
namespace {

// Registry-owned artifact schemas keep payload identity separate from file path
// so contract_id and schema_id based artifacts can use the same lookup surface.
constexpr std::array<ArtifactSchemaContract, 8> kBaseArtifactSchemaContracts{{
    {"objc3c-public-command-contract-v1",
     "contract_id",
     "objc3c-public-command-contract-v1",
     "https://slopjective-c.dev/schemas/objc3c-public-command-contract-v1.schema.json",
     "schemas/objc3c-public-command-contract-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "public-command-contract"},
    {"objc3c-capability-matrix-v1",
     "schema_version",
     "objc3c-capability-matrix-v1",
     "https://objc3c.dev/schemas/objc3c-capability-matrix-v1.schema.json",
     "schemas/objc3c-capability-matrix-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "capability-support"},
    {"objc3c-capability-evidence-map-v1",
     "schema_version",
     "objc3c-capability-evidence-map-v1",
     "https://objc3c.dev/schemas/objc3c-capability-evidence-map-v1.schema.json",
     "schemas/objc3c-capability-evidence-map-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "capability-support"},
    {"objc3c-validation-acceptance-artifact-index-v1",
     "contract_id",
     "objc3c.validation.acceptance.artifact.index.v1",
     "https://slopjective-c.dev/schemas/objc3c-validation-acceptance-artifact-index-v1.schema.json",
     "schemas/objc3c-validation-acceptance-artifact-index-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "validation-acceptance"},
    {"objc3c-public-conformance-summary-v1",
     "contract_id",
     "objc3c.public_conformance_reporting.summary.v1",
     "https://schemas.slopjective.local/objc3c-public-conformance-summary-v1.schema.json",
     "schemas/objc3c-public-conformance-summary-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "public-conformance"},
    {"objc3c-public-conformance-scorecard-v1",
     "contract_id",
     "objc3c.public_conformance_reporting.scorecard.summary.v1",
     "https://schemas.slopjective.local/objc3c-public-conformance-scorecard-v1.schema.json",
     "schemas/objc3c-public-conformance-scorecard-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "public-conformance"},
    {"objc3c-runtime-performance-telemetry-v1",
     "contract_id",
     "objc3c.runtime.performance.telemetry.v1",
     "https://objc3c.dev/schemas/objc3c-runtime-performance-telemetry-v1.schema.json",
     "schemas/objc3c-runtime-performance-telemetry-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "runtime-performance"},
    {"objc3-conformance-dashboard-status/v1",
     "schema_id",
     "objc3-conformance-dashboard-status/v1",
     "https://schemas.slopjective.local/objc3-conformance-dashboard-status-v1.schema.json",
     "schemas/objc3-conformance-dashboard-status-v1.schema.json",
     "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp",
     "conformance-dashboard"},
}};

const std::vector<ArtifactSchemaContract> kArtifactSchemaContracts = [] {
  std::vector<ArtifactSchemaContract> contracts(
      kBaseArtifactSchemaContracts.begin(),
      kBaseArtifactSchemaContracts.end());
  const std::span<const ArtifactSchemaContract> release_readiness_contracts =
      ReleaseReadinessSchemaContracts();
  contracts.insert(contracts.end(),
                   release_readiness_contracts.begin(),
                   release_readiness_contracts.end());
  return contracts;
}();

}  // namespace

std::span<const ArtifactSchemaContract> ArtifactSchemaContractEntries() {
  return std::span<const ArtifactSchemaContract>(
      kArtifactSchemaContracts.data(), kArtifactSchemaContracts.size());
}

}  // namespace objc3::artifacts::json
