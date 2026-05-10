#include "io/json/json_schema_any_of_mismatch_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaAnyOfMismatch(
    const JsonSchemaAnyOfCandidateMatch &match,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!match.matched && !match.schema_failed) {
    AddJsonSchemaPayloadError(
        result, "any_of", instance_path,
        JsonSchemaKeywordPath(schema_path, "anyOf"),
        "value did not match any allowed schema");
  }
}

}  // namespace objc3::io::json
