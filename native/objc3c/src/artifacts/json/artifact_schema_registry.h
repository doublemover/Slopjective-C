#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace objc3::artifacts::json {

struct ArtifactSchemaContract {
  std::string_view schema_id;
  std::string_view schema_path;
};

[[nodiscard]] std::vector<ArtifactSchemaContract> ListArtifactSchemaContracts();
[[nodiscard]] std::optional<ArtifactSchemaContract> LookupArtifactSchemaContract(std::string_view schema_id);
[[nodiscard]] std::optional<std::string> LookupArtifactSchemaPath(std::string_view schema_id);

}  // namespace objc3::artifacts::json
