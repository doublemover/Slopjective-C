#include "io/json/json_schema_array_cardinality_min_items_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaArrayMinItemsKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *min_items = schema.Find("minItems");
  if (min_items == nullptr || !payload.IsArray()) {
    return nullptr;
  }
  if (!min_items->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_min_items",
        JsonSchemaKeywordPath(schema_path, "minItems"),
        "minItems must be a number");
    return nullptr;
  }
  return min_items;
}

}  // namespace objc3::io::json
