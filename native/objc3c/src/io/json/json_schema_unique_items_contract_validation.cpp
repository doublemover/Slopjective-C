#include "io/json/json_schema_unique_items_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaUniqueItemsContract(const JsonValue &schema,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  if (const JsonValue *unique = schema.Find("uniqueItems");
      unique != nullptr && !unique->IsBool()) {
    AddJsonSchemaContractError(
        result, "invalid_unique_items",
        JsonSchemaKeywordPath(schema_path, "uniqueItems"),
        "uniqueItems must be a boolean");
  }
}

}  // namespace objc3::io::json
