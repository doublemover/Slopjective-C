#include "io/json/json_schema_string_constraint_pattern_validation.h"

#include <regex>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringPatternConstraint(
    const JsonValue &schema, const JsonValue &payload,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result) {
  if (const auto pattern = schema.GetString("pattern");
      pattern.has_value() && payload.IsString()) {
    try {
      if (!std::regex_search(payload.AsString(), std::regex(*pattern))) {
        AddJsonSchemaPayloadError(
            result, "pattern", instance_path,
            JsonSchemaKeywordPath(schema_path, "pattern"),
            "string did not match pattern");
      }
    } catch (const std::regex_error &) {
      AddJsonSchemaContractError(
          result, "invalid_pattern",
          JsonSchemaKeywordPath(schema_path, "pattern"),
          "pattern is not a valid regular expression");
    }
  }
}

}  // namespace objc3::io::json
