#include "io/json/json_schema_numeric_constraint_maximum_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaMaximumKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *maximum = schema.Find("maximum");
  if (maximum == nullptr || !payload.IsNumber()) {
    return nullptr;
  }
  if (!maximum->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_maximum",
        JsonSchemaKeywordPath(schema_path, "maximum"),
        "maximum must be a number");
    return nullptr;
  }
  return maximum;
}

}  // namespace objc3::io::json
