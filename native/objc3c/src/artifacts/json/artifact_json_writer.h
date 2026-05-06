#pragma once

#include <string>

#include "artifacts/json/artifact_json_model.h"

namespace objc3::artifacts::json {

[[nodiscard]] std::string RenderArtifactJson(const ArtifactJsonDocument &document);

}  // namespace objc3::artifacts::json
