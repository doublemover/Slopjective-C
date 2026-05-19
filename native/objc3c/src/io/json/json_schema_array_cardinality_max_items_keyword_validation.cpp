#include "io/json/json_schema_array_cardinality_max_items_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaArrayMaxItemsKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *max_items = schema.Find("maxItems");
  if (max_items == nullptr || !payload.IsArray()) {
    return nullptr;
  }
  if (!max_items->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_max_items",
        JsonSchemaKeywordPath(schema_path, "maxItems"),
        "maxItems must be a number");
    return nullptr;
  }
  return max_items;
}

}  // namespace objc3::io::json
