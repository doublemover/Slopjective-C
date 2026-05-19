#include "artifacts/json/artifact_json_writer.h"

#include <sstream>

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
