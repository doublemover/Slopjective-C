#pragma once

#include <string>

#include "artifacts/json/artifact_json_model.h"
#include "artifacts/json/artifact_schema_registry.h"

namespace objc3::artifacts::json {

struct ArtifactJsonPublicationRequest {
  std::string schema_id;
  objc3::io::json::JsonValue payload;
};

struct ArtifactJsonPublicationResult {
  bool ok = false;
  std::string artifact_json;
  std::string error;
  std::string payload_id;
  ArtifactSchemaContract schema_contract;
};

[[nodiscard]] bool TryBuildRegisteredArtifactJsonDocument(
    const ArtifactJsonPublicationRequest &request,
    ArtifactJsonDocument &document,
    ArtifactSchemaContract &contract,
    std::string &payload_id,
    std::string &error);

}  // namespace objc3::artifacts::json
