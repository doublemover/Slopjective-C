#include "io/json/json_schema_array_uniqueness_validation.h"

#include "io/json/json_schema_array_uniqueness_duplicates_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayUniqueness(const JsonValue &schema,
                                       const JsonValue &payload,
                                       const std::string &instance_path,
                                       const std::string &schema_path,
                                       JsonSchemaResult &result) {
  const JsonValue *unique_items = schema.Find("uniqueItems");
  if (unique_items != nullptr && payload.IsArray()) {
    if (!unique_items->IsBool()) {
      AddJsonSchemaContractError(
          result, "invalid_unique_items",
          JsonSchemaKeywordPath(schema_path, "uniqueItems"),
          "uniqueItems must be a boolean");
      return false;
    }
    if (!unique_items->AsBool()) {
      return false;
    }
    ValidateJsonSchemaArrayUniqueItemDuplicates(
        payload.AsArray(), instance_path,
        JsonSchemaKeywordPath(schema_path, "uniqueItems"), result);
  }
  return true;
}

}  // namespace objc3::io::json
