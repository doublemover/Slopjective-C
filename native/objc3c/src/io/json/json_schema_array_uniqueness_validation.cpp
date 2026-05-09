#include "io/json/json_schema_array_uniqueness_validation.h"

#include <cstddef>

#include "io/json/json_equivalence.h"
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
    const JsonValue::Array &array = payload.AsArray();
    bool duplicate = false;
    for (std::size_t i = 0; i < array.size(); ++i) {
      for (std::size_t j = i + 1; j < array.size(); ++j) {
        if (JsonEquals(array[i], array[j])) {
          duplicate = true;
          break;
        }
      }
      if (duplicate) {
        AddJsonSchemaPayloadError(
            result, "unique_items", instance_path,
            JsonSchemaKeywordPath(schema_path, "uniqueItems"),
            "array contains duplicate items");
        break;
      }
    }
  }
  return true;
}

}  // namespace objc3::io::json
