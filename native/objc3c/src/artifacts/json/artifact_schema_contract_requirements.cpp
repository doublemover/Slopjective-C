#include "artifacts/json/artifact_schema_registry.h"

#include <string>

namespace objc3::artifacts::json {

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
    error =
        "artifact schema registry contains schema entries without schema paths";
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

bool RequireArtifactSchemaContractByPayloadId(
    std::string_view payload_id,
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
