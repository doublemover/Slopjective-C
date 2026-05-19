#include "artifacts/json/artifact_json_publication_contract.h"

#include <optional>

namespace objc3::artifacts::json {

bool TryBuildRegisteredArtifactJsonDocument(
    const ArtifactJsonPublicationRequest &request,
    ArtifactJsonDocument &document,
    ArtifactSchemaContract &contract,
    std::string &payload_id,
    std::string &error) {
  document = {};
  payload_id.clear();
  if (!RequireArtifactSchemaContract(request.schema_id, contract, error)) {
    return false;
  }
  if (!request.payload.IsObject()) {
    error = "artifact payload for schema_id " + request.schema_id +
            " is not a JSON object";
    return false;
  }
  const std::optional<std::string> resolved_payload_id =
      request.payload.GetString(contract.payload_id_field);
  if (!resolved_payload_id.has_value() ||
      *resolved_payload_id != contract.payload_id_value) {
    error = "artifact payload id for schema_id " + request.schema_id +
            " does not match registered " +
            std::string(contract.payload_id_field);
    return false;
  }
  payload_id = *resolved_payload_id;
  document.schema_id = std::string(contract.schema_id);
  document.payload = request.payload;
  error.clear();
  return true;
}

}  // namespace objc3::artifacts::json
