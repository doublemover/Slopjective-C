#include "io/json/json_schema_numeric_nonnegative_array_item_count_max_items_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaNonnegativeMaxItemsContract(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("maxItems");
  if (value == nullptr) {
    return;
  }
  if (!value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, "maxItems"),
        "maxItems must be a number");
  }
  if (value->IsNumber() && value->AsNumber() < 0.0) {
    AddJsonSchemaContractError(
        result, "invalid_nonnegative_keyword",
        JsonSchemaKeywordPath(schema_path, "maxItems"),
        "maxItems must be zero or greater");
  }
}

}  // namespace objc3::io::json
