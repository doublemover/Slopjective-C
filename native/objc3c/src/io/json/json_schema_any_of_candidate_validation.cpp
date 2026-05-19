#include "io/json/json_schema_any_of_candidate_validation.h"

#include <cstddef>

#include "io/json/json_schema_any_of_candidate_probe_validation.h"
#include "io/json/json_schema_any_of_candidate_schema_issue_validation.h"

namespace objc3::io::json {

JsonSchemaAnyOfCandidateMatch ValidateJsonSchemaAnyOfCandidates(
    const JsonValue &schema_root, const JsonValue::Array &candidates,
    const JsonValue &payload, const std::string &instance_path,
    const std::string &schema_path, JsonSchemaResult &result) {
  JsonSchemaAnyOfCandidateMatch match;
  for (std::size_t i = 0; i < candidates.size(); ++i) {
    JsonSchemaResult probe = ValidateJsonSchemaAnyOfCandidateProbe(
        schema_root, candidates[i], payload, instance_path, schema_path, i);
    if (PropagateJsonSchemaAnyOfCandidateSchemaIssues(probe, result)) {
      match.schema_failed = true;
      continue;
    }
    if (probe.ok) {
      match.matched = true;
      break;
    }
  }
  return match;
}

}  // namespace objc3::io::json
