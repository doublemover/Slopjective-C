#include "io/json/json_schema_numeric_nonnegative_array_item_count_min_items_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaNonnegativeMinItemsContract(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("minItems");
  if (value == nullptr) {
    return;
  }
  if (!value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, "minItems"),
        "minItems must be a number");
  }
  if (value->IsNumber() && value->AsNumber() < 0.0) {
    AddJsonSchemaContractError(
        result, "invalid_nonnegative_keyword",
        JsonSchemaKeywordPath(schema_path, "minItems"),
        "minItems must be zero or greater");
  }
}

}  // namespace objc3::io::json
