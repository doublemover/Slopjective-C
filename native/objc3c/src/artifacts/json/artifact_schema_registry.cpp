#include "artifacts/json/artifact_schema_registry.h"

#include "artifacts/json/artifact_schema_contract_table.h"

#include <set>
#include <string>
#include <vector>

namespace objc3::artifacts::json {

std::vector<ArtifactSchemaContract> ListArtifactSchemaContracts() {
  const std::span<const ArtifactSchemaContract> contracts =
      ArtifactSchemaContractEntries();
  return std::vector<ArtifactSchemaContract>(contracts.begin(),
                                             contracts.end());
}

std::vector<ArtifactSchemaContract> ListArtifactSchemaContractsByFamily(
    std::string_view artifact_family) {
  std::vector<ArtifactSchemaContract> contracts;
  for (const ArtifactSchemaContract &contract :
       ArtifactSchemaContractEntries()) {
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
  const std::span<const ArtifactSchemaContract> contracts =
      ArtifactSchemaContractEntries();
  for (const ArtifactSchemaContract &contract : contracts) {
    schema_ids.insert(std::string(contract.schema_id));
    payload_ids.insert(std::string(contract.payload_id_value));
    artifact_families.insert(std::string(contract.artifact_family));
    summary.schema_paths_present =
        summary.schema_paths_present && !contract.schema_path.empty() &&
        !contract.schema_uri.empty();
  }
  summary.schema_count = contracts.size();
  summary.artifact_family_count = artifact_families.size();
  summary.schema_ids_lexicographic.assign(schema_ids.begin(), schema_ids.end());
  summary.payload_ids_lexicographic.assign(payload_ids.begin(), payload_ids.end());
  summary.artifact_families_lexicographic.assign(artifact_families.begin(),
                                                 artifact_families.end());
  summary.schema_ids_unique =
      summary.schema_ids_lexicographic.size() == contracts.size();
  summary.payload_ids_unique =
      summary.payload_ids_lexicographic.size() == contracts.size();
  return summary;
}

std::optional<ArtifactSchemaContract> LookupArtifactSchemaContract(std::string_view schema_id) {
  for (const ArtifactSchemaContract &contract :
       ArtifactSchemaContractEntries()) {
    if (contract.schema_id == schema_id) {
      return contract;
    }
  }
  return std::nullopt;
}

std::optional<ArtifactSchemaContract> LookupArtifactSchemaContractByPayloadId(
    std::string_view payload_id) {
  for (const ArtifactSchemaContract &contract :
       ArtifactSchemaContractEntries()) {
    if (contract.payload_id_value == payload_id) {
      return contract;
    }
  }
  return std::nullopt;
}

std::optional<ArtifactSchemaContract> LookupArtifactSchemaContractByFamilyAndPayloadId(
    std::string_view artifact_family,
    std::string_view payload_id) {
  for (const ArtifactSchemaContract &contract :
       ArtifactSchemaContractEntries()) {
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
