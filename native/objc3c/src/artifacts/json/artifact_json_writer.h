#pragma once

#include <string>

#include "artifacts/json/artifact_json_model.h"
#include "artifacts/json/artifact_json_publication_contract.h"

namespace objc3::artifacts::json {

[[nodiscard]] std::string RenderArtifactJson(const ArtifactJsonDocument &document);
[[nodiscard]] ArtifactJsonPublicationResult PublishRegisteredArtifactJson(
    const ArtifactJsonPublicationRequest &request);
[[nodiscard]] bool TryRenderRegisteredArtifactJson(
    const ArtifactJsonDocument &document,
    std::string &artifact_json,
    std::string &error);

}  // namespace objc3::artifacts::json
