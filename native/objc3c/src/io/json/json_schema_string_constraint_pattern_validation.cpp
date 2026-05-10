#include "io/json/json_schema_string_constraint_pattern_validation.h"

#include "io/json/json_schema_string_constraint_pattern_keyword_validation.h"
#include "io/json/json_schema_string_constraint_pattern_payload_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringPatternConstraint(
    const JsonValue &schema, const JsonValue &payload,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonSchemaStringPatternKeywordValidation pattern =
      ValidateJsonSchemaStringPatternKeyword(schema, payload, schema_path,
                                             result);
  if (!pattern.regex.has_value()) {
    return;
  }
  ValidateJsonSchemaStringPatternPayload(*pattern.regex, payload, instance_path,
                                         schema_path, result);
}

}  // namespace objc3::io::json
