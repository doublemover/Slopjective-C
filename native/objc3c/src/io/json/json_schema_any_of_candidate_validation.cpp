#include "io/json/json_schema_any_of_candidate_validation.h"

#include <cstddef>
#include <utility>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

JsonSchemaAnyOfCandidateMatch ValidateJsonSchemaAnyOfCandidates(
    const JsonValue &schema_root, const JsonValue::Array &candidates,
    const JsonValue &payload, const std::string &instance_path,
    const std::string &schema_path, JsonSchemaResult &result) {
  JsonSchemaAnyOfCandidateMatch match;
  for (std::size_t i = 0; i < candidates.size(); ++i) {
    JsonSchemaResult probe;
    ValidateJsonSchemaNode(schema_root, candidates[i], payload, instance_path,
                           JsonSchemaArrayElementPath(schema_path, "anyOf", i),
                           probe);
    if (HasJsonSchemaContractIssue(probe)) {
      match.schema_failed = true;
      for (JsonSchemaIssue &issue : probe.errors) {
        if (issue.domain == "schema") {
          AppendJsonSchemaIssue(result, std::move(issue));
        }
      }
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
