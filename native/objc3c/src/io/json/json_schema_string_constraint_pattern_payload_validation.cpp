#include "io/json/json_schema_string_constraint_pattern_payload_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringPatternPayload(
    const std::regex &pattern,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!std::regex_search(payload.AsString(), pattern)) {
    AddJsonSchemaPayloadError(
        result, "pattern", instance_path,
        JsonSchemaKeywordPath(schema_path, "pattern"),
        "string did not match pattern");
  }
}

}  // namespace objc3::io::json
