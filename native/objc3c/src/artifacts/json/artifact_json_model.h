#pragma once

#include <string>

#include "io/json/json_value.h"

namespace objc3::artifacts::json {

struct ArtifactJsonDocument {
  std::string schema_id;
  objc3::io::json::JsonValue payload;
};

}  // namespace objc3::artifacts::json
