#include "io/json/json_schema_numeric_constraint_minimum_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaMinimumKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *minimum = schema.Find("minimum");
  if (minimum == nullptr || !payload.IsNumber()) {
    return nullptr;
  }
  if (!minimum->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_minimum",
        JsonSchemaKeywordPath(schema_path, "minimum"),
        "minimum must be a number");
    return nullptr;
  }
  return minimum;
}

}  // namespace objc3::io::json
