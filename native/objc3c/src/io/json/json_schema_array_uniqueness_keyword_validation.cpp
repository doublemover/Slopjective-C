#include "io/json/json_schema_array_uniqueness_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaArrayUniquenessKeywordValidation
ValidateJsonSchemaArrayUniqueItemsKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *unique_items = schema.Find("uniqueItems");
  if (unique_items == nullptr || !payload.IsArray()) {
    return {};
  }
  if (!unique_items->IsBool()) {
    AddJsonSchemaContractError(
        result, "invalid_unique_items",
        JsonSchemaKeywordPath(schema_path, "uniqueItems"),
        "uniqueItems must be a boolean");
    JsonSchemaArrayUniquenessKeywordValidation invalid;
    invalid.valid = false;
    return invalid;
  }
  if (!unique_items->AsBool()) {
    JsonSchemaArrayUniquenessKeywordValidation disabled;
    disabled.valid = false;
    return disabled;
  }
  JsonSchemaArrayUniquenessKeywordValidation valid;
  valid.scan_duplicates = true;
  return valid;
}

}  // namespace objc3::io::json
