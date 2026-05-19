#include "artifacts/json/artifact_schema_registry.h"

#include "artifacts/json/artifact_schema_contract_table.h"

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

}  // namespace objc3::artifacts::json
