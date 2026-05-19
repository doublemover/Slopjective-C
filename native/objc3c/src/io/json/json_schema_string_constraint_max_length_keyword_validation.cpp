#include "io/json/json_schema_string_constraint_max_length_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaStringMaxLengthKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *max_length = schema.Find("maxLength");
  if (max_length == nullptr || !payload.IsString()) {
    return nullptr;
  }
  if (!max_length->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_max_length",
        JsonSchemaKeywordPath(schema_path, "maxLength"),
        "maxLength must be a number");
    return nullptr;
  }
  if (max_length->AsNumber() < 0.0) {
    return nullptr;
  }
  return max_length;
}

}  // namespace objc3::io::json
