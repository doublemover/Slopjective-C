#include "io/json/json_schema_array_cardinality_max_items_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayMaxItems(const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *max_items = schema.Find("maxItems");
  if (max_items != nullptr && payload.IsArray()) {
    if (!max_items->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_max_items",
          JsonSchemaKeywordPath(schema_path, "maxItems"),
          "maxItems must be a number");
    } else if (payload.AsArray().size() >
               static_cast<std::size_t>(max_items->AsNumber())) {
      AddJsonSchemaPayloadError(
          result, "max_items", instance_path,
          JsonSchemaKeywordPath(schema_path, "maxItems"),
          "array has too many items");
    }
  }
}

}  // namespace objc3::io::json
