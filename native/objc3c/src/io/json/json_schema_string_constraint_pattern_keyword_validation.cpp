#include "io/json/json_schema_string_constraint_pattern_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaStringPatternKeywordValidation ValidateJsonSchemaStringPatternKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  JsonSchemaStringPatternKeywordValidation validation;
  const JsonValue *pattern = schema.Find("pattern");
  if (pattern == nullptr || !payload.IsString()) {
    return validation;
  }
  if (!pattern->IsString()) {
    return validation;
  }
  try {
    validation.regex.emplace(pattern->AsString());
  } catch (const std::regex_error &) {
    AddJsonSchemaContractError(
        result, "invalid_pattern",
        JsonSchemaKeywordPath(schema_path, "pattern"),
        "pattern is not a valid regular expression");
  }
  return validation;
}

}  // namespace objc3::io::json
