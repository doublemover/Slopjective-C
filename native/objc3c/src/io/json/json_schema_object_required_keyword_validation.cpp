#include "io/json/json_schema_object_required_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaObjectRequiredKeyword(
    const JsonValue &schema, const JsonValue &payload,
    const std::string &schema_path, JsonSchemaResult &result) {
  const JsonValue *required = schema.Find("required");
  if (required == nullptr || !payload.IsObject()) {
    return nullptr;
  }
  if (!required->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_required",
        JsonSchemaKeywordPath(schema_path, "required"),
        "required must be an array of property-name strings");
    return nullptr;
  }
  return required;
}

}  // namespace objc3::io::json
