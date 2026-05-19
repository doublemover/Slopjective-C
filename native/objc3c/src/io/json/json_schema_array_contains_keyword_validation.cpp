#include "io/json/json_schema_array_contains_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaArrayContainsKeywordValidation
ValidateJsonSchemaArrayContainsKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *contains = schema.Find("contains");
  if (contains == nullptr || !payload.IsArray()) {
    return {};
  }
  if (!contains->IsObject()) {
    AddJsonSchemaContractError(
        result, "invalid_contains",
        JsonSchemaKeywordPath(schema_path, "contains"),
        "contains must be a schema object");
    JsonSchemaArrayContainsKeywordValidation invalid;
    invalid.valid = false;
    return invalid;
  }
  JsonSchemaArrayContainsKeywordValidation valid;
  valid.value = contains;
  return valid;
}

}  // namespace objc3::io::json
