#pragma once

#include <cstddef>
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

struct ArtifactSchemaRegistrySummary {
  std::size_t schema_count = 0;
  std::size_t artifact_family_count = 0;
  std::vector<std::string> schema_ids_lexicographic;
  std::vector<std::string> payload_ids_lexicographic;
  std::vector<std::string> artifact_families_lexicographic;
  bool schema_ids_unique = false;
  bool payload_ids_unique = false;
  bool schema_paths_present = false;
};

[[nodiscard]] std::vector<ArtifactSchemaContract> ListArtifactSchemaContracts();
[[nodiscard]] std::vector<ArtifactSchemaContract> ListArtifactSchemaContractsByFamily(
    std::string_view artifact_family);
[[nodiscard]] ArtifactSchemaRegistrySummary BuildArtifactSchemaRegistrySummary();
[[nodiscard]] std::optional<ArtifactSchemaContract> LookupArtifactSchemaContract(std::string_view schema_id);
[[nodiscard]] std::optional<ArtifactSchemaContract> LookupArtifactSchemaContractByPayloadId(
    std::string_view payload_id);
[[nodiscard]] std::optional<ArtifactSchemaContract> LookupArtifactSchemaContractByFamilyAndPayloadId(
    std::string_view artifact_family,
    std::string_view payload_id);
[[nodiscard]] std::optional<std::string> LookupArtifactSchemaPath(std::string_view schema_id);
[[nodiscard]] bool RequireArtifactSchemaRegistryIntegrity(std::string &error);
[[nodiscard]] bool RequireArtifactSchemaContract(
    std::string_view schema_id,
    ArtifactSchemaContract &contract,
    std::string &error);
[[nodiscard]] bool RequireArtifactSchemaContractByPayloadId(
    std::string_view payload_id,
    ArtifactSchemaContract &contract,
    std::string &error);

}  // namespace objc3::artifacts::json
