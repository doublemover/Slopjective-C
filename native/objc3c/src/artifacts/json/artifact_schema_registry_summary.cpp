#include "artifacts/json/artifact_schema_registry.h"

#include <set>
#include <string>

#include "artifacts/json/artifact_schema_contract_table.h"

namespace objc3::artifacts::json {

ArtifactSchemaRegistrySummary BuildArtifactSchemaRegistrySummary() {
  ArtifactSchemaRegistrySummary summary;
  std::set<std::string> schema_ids;
  std::set<std::string> payload_ids;
  std::set<std::string> artifact_families;
  summary.schema_paths_present = true;
  const std::span<const ArtifactSchemaContract> contracts =
      ArtifactSchemaContractEntries();
  for (const ArtifactSchemaContract &contract : contracts) {
    schema_ids.insert(std::string(contract.schema_id));
    payload_ids.insert(std::string(contract.payload_id_value));
    artifact_families.insert(std::string(contract.artifact_family));
    summary.schema_paths_present =
        summary.schema_paths_present && !contract.schema_path.empty() &&
        !contract.schema_uri.empty();
  }
  summary.schema_count = contracts.size();
  summary.artifact_family_count = artifact_families.size();
  summary.schema_ids_lexicographic.assign(schema_ids.begin(), schema_ids.end());
  summary.payload_ids_lexicographic.assign(payload_ids.begin(),
                                           payload_ids.end());
  summary.artifact_families_lexicographic.assign(artifact_families.begin(),
                                                 artifact_families.end());
  summary.schema_ids_unique =
      summary.schema_ids_lexicographic.size() == contracts.size();
  summary.payload_ids_unique =
      summary.payload_ids_lexicographic.size() == contracts.size();
  return summary;
}

}  // namespace objc3::artifacts::json
