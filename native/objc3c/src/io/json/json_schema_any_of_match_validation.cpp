#include "io/json/json_schema_any_of_match_validation.h"

#include "io/json/json_schema_any_of_candidate_validation.h"

namespace objc3::io::json {

JsonSchemaAnyOfCandidateMatch ValidateJsonSchemaAnyOfMatch(
    const JsonValue &schema_root,
    const JsonValue::Array &candidates,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  return ValidateJsonSchemaAnyOfCandidates(schema_root, candidates, payload,
                                           instance_path, schema_path, result);
}

}  // namespace objc3::io::json
