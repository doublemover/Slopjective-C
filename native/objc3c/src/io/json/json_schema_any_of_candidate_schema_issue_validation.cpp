#include "io/json/json_schema_any_of_candidate_schema_issue_validation.h"

#include <utility>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool PropagateJsonSchemaAnyOfCandidateSchemaIssues(
    JsonSchemaResult &probe,
    JsonSchemaResult &result) {
  if (!HasJsonSchemaContractIssue(probe)) {
    return false;
  }
  for (JsonSchemaIssue &issue : probe.errors) {
    if (issue.domain == "schema") {
      AppendJsonSchemaIssue(result, std::move(issue));
    }
  }
  return true;
}

}  // namespace objc3::io::json
