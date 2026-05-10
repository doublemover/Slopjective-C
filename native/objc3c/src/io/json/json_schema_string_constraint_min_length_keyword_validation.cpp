#include "io/json/json_schema_string_constraint_min_length_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaStringMinLengthKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *min_length = schema.Find("minLength");
  if (min_length == nullptr || !payload.IsString()) {
    return nullptr;
  }
  if (!min_length->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_min_length",
        JsonSchemaKeywordPath(schema_path, "minLength"),
        "minLength must be a number");
    return nullptr;
  }
  if (min_length->AsNumber() < 0.0) {
    return nullptr;
  }
  return min_length;
}

}  // namespace objc3::io::json
