#include "artifacts/json/artifact_json_writer.h"

#include <optional>
#include <sstream>

#include "artifacts/json/artifact_schema_registry.h"
#include "io/json/json_writer.h"

namespace objc3::artifacts::json {

std::string RenderArtifactJson(const ArtifactJsonDocument &document) {
  std::ostringstream out;
  objc3::io::json::JsonObjectWriter root(out);
  root.StringField("schema_id", document.schema_id);
  root.ValueField("payload", document.payload);
  root.End();
  return out.str();
}

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

ArtifactJsonPublicationResult PublishRegisteredArtifactJson(
    const ArtifactJsonPublicationRequest &request) {
  ArtifactJsonPublicationResult result;
  ArtifactJsonDocument document;
  if (!TryBuildRegisteredArtifactJsonDocument(request, document,
                                              result.schema_contract,
                                              result.payload_id,
                                              result.error)) {
    return result;
  }
  result.artifact_json = RenderArtifactJson(document);
  result.ok = true;
  result.error.clear();
  return result;
}

bool TryRenderRegisteredArtifactJson(const ArtifactJsonDocument &document,
                                     std::string &artifact_json,
                                     std::string &error) {
  artifact_json.clear();
  ArtifactJsonPublicationRequest request;
  request.schema_id = document.schema_id;
  request.payload = document.payload;
  const ArtifactJsonPublicationResult publication =
      PublishRegisteredArtifactJson(request);
  if (!publication.ok) {
    error = publication.error;
    return false;
  }
  artifact_json = publication.artifact_json;
  error.clear();
  return true;
}

}  // namespace objc3::artifacts::json
