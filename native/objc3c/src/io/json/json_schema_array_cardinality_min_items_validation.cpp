#include "io/json/json_schema_array_cardinality_min_items_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayMinItems(const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *min_items = schema.Find("minItems");
  if (min_items != nullptr && payload.IsArray()) {
    if (!min_items->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_min_items",
          JsonSchemaKeywordPath(schema_path, "minItems"),
          "minItems must be a number");
    } else if (payload.AsArray().size() <
               static_cast<std::size_t>(min_items->AsNumber())) {
      AddJsonSchemaPayloadError(
          result, "min_items", instance_path,
          JsonSchemaKeywordPath(schema_path, "minItems"),
          "array has too few items");
    }
  }
}

}  // namespace objc3::io::json
