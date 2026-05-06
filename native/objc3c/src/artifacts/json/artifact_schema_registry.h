#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace objc3::artifacts::json {

[[nodiscard]] std::optional<std::string> LookupArtifactSchemaPath(std::string_view schema_id);

}  // namespace objc3::artifacts::json
