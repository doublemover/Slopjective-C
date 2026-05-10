#include "io/json/json_schema_array_items_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayItemsKeyword(
    const JsonValue &items,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!items.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_items",
                               JsonSchemaKeywordPath(schema_path, "items"),
                               "items must be a schema object");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
