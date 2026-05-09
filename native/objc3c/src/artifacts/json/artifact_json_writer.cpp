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

bool TryRenderRegisteredArtifactJson(const ArtifactJsonDocument &document,
                                     std::string &artifact_json,
                                     std::string &error) {
  artifact_json.clear();
  ArtifactSchemaContract contract;
  if (!RequireArtifactSchemaContract(document.schema_id, contract, error)) {
    return false;
  }
  if (!document.payload.IsObject()) {
    error = "artifact payload for schema_id " + document.schema_id +
            " is not a JSON object";
    return false;
  }
  const std::optional<std::string> payload_id =
      document.payload.GetString(contract.payload_id_field);
  if (!payload_id.has_value() || *payload_id != contract.payload_id_value) {
    error = "artifact payload id for schema_id " + document.schema_id +
            " does not match registered " +
            std::string(contract.payload_id_field);
    return false;
  }
  artifact_json = RenderArtifactJson(document);
  error.clear();
  return true;
}

}  // namespace objc3::artifacts::json
