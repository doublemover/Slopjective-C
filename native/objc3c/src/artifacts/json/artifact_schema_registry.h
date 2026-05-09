#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace objc3::artifacts::json {

struct ArtifactSchemaContract {
  std::string_view schema_id;
  std::string_view payload_id_field;
  std::string_view payload_id_value;
  std::string_view schema_uri;
  std::string_view schema_path;
  std::string_view owner;
  std::string_view artifact_family;
};

[[nodiscard]] std::vector<ArtifactSchemaContract> ListArtifactSchemaContracts();
[[nodiscard]] std::optional<ArtifactSchemaContract> LookupArtifactSchemaContract(std::string_view schema_id);
[[nodiscard]] std::optional<ArtifactSchemaContract> LookupArtifactSchemaContractByPayloadId(
    std::string_view payload_id);
[[nodiscard]] std::optional<std::string> LookupArtifactSchemaPath(std::string_view schema_id);
[[nodiscard]] bool RequireArtifactSchemaContract(
    std::string_view schema_id,
    ArtifactSchemaContract &contract,
    std::string &error);
[[nodiscard]] bool RequireArtifactSchemaContractByPayloadId(
    std::string_view payload_id,
    ArtifactSchemaContract &contract,
    std::string &error);

}  // namespace objc3::artifacts::json
