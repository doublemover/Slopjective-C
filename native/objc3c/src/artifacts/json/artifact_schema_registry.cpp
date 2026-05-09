#include "artifacts/json/artifact_schema_registry.h"

#include <array>
#include <set>
#include <string>
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

std::vector<ArtifactSchemaContract> ListArtifactSchemaContractsByFamily(
    std::string_view artifact_family) {
  std::vector<ArtifactSchemaContract> contracts;
  for (const ArtifactSchemaContract &contract : kSchemas) {
    if (contract.artifact_family == artifact_family) {
      contracts.push_back(contract);
    }
  }
  return contracts;
}

ArtifactSchemaRegistrySummary BuildArtifactSchemaRegistrySummary() {
  ArtifactSchemaRegistrySummary summary;
  std::set<std::string> schema_ids;
  std::set<std::string> payload_ids;
  std::set<std::string> artifact_families;
  summary.schema_paths_present = true;
  for (const ArtifactSchemaContract &contract : kSchemas) {
    schema_ids.insert(std::string(contract.schema_id));
    payload_ids.insert(std::string(contract.payload_id_value));
    artifact_families.insert(std::string(contract.artifact_family));
    summary.schema_paths_present =
        summary.schema_paths_present && !contract.schema_path.empty() &&
        !contract.schema_uri.empty();
  }
  summary.schema_count = kSchemas.size();
  summary.artifact_family_count = artifact_families.size();
  summary.schema_ids_lexicographic.assign(schema_ids.begin(), schema_ids.end());
  summary.payload_ids_lexicographic.assign(payload_ids.begin(), payload_ids.end());
  summary.artifact_families_lexicographic.assign(artifact_families.begin(),
                                                 artifact_families.end());
  summary.schema_ids_unique =
      summary.schema_ids_lexicographic.size() == kSchemas.size();
  summary.payload_ids_unique =
      summary.payload_ids_lexicographic.size() == kSchemas.size();
  return summary;
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

std::optional<ArtifactSchemaContract> LookupArtifactSchemaContractByFamilyAndPayloadId(
    std::string_view artifact_family,
    std::string_view payload_id) {
  for (const ArtifactSchemaContract &contract : kSchemas) {
    if (contract.artifact_family == artifact_family &&
        contract.payload_id_value == payload_id) {
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

bool RequireArtifactSchemaRegistryIntegrity(std::string &error) {
  const ArtifactSchemaRegistrySummary summary =
      BuildArtifactSchemaRegistrySummary();
  if (!summary.schema_ids_unique) {
    error = "artifact schema registry contains duplicate schema_id values";
    return false;
  }
  if (!summary.payload_ids_unique) {
    error = "artifact schema registry contains duplicate payload id values";
    return false;
  }
  if (!summary.schema_paths_present) {
    error = "artifact schema registry contains schema entries without schema paths";
    return false;
  }
  error.clear();
  return true;
}

bool RequireArtifactSchemaContract(std::string_view schema_id,
                                   ArtifactSchemaContract &contract,
                                   std::string &error) {
  if (schema_id.empty()) {
    error = "artifact schema_id is empty";
    return false;
  }
  const std::optional<ArtifactSchemaContract> found =
      LookupArtifactSchemaContract(schema_id);
  if (!found.has_value()) {
    error = "unsupported artifact schema_id: " + std::string(schema_id);
    return false;
  }
  contract = *found;
  error.clear();
  return true;
}

bool RequireArtifactSchemaContractByPayloadId(std::string_view payload_id,
                                              ArtifactSchemaContract &contract,
                                              std::string &error) {
  if (payload_id.empty()) {
    error = "artifact payload id is empty";
    return false;
  }
  const std::optional<ArtifactSchemaContract> found =
      LookupArtifactSchemaContractByPayloadId(payload_id);
  if (!found.has_value()) {
    error = "unsupported artifact payload id: " + std::string(payload_id);
    return false;
  }
  contract = *found;
  error.clear();
  return true;
}

}  // namespace objc3::artifacts::json
