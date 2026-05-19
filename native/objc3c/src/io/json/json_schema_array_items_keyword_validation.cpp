#include "io/json/json_schema_array_items_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaArrayItemsKeywordValidation
ValidateJsonSchemaArrayItemsKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *items = schema.Find("items");
  if (items == nullptr || !payload.IsArray()) {
    return {};
  }
  if (!items->IsObject()) {
    AddJsonSchemaContractError(result, "invalid_items",
                               JsonSchemaKeywordPath(schema_path, "items"),
                               "items must be a schema object");
    JsonSchemaArrayItemsKeywordValidation invalid;
    invalid.valid = false;
    return invalid;
  }
  JsonSchemaArrayItemsKeywordValidation valid;
  valid.value = items;
  return valid;
}

}  // namespace objc3::io::json
