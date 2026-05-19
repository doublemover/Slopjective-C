#include "io/json/json_schema_array_cardinality_min_items_payload_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayMinItemsPayload(
    const JsonValue &min_items,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (payload.AsArray().size() <
      static_cast<std::size_t>(min_items.AsNumber())) {
    AddJsonSchemaPayloadError(
        result, "min_items", instance_path,
        JsonSchemaKeywordPath(schema_path, "minItems"),
        "array has too few items");
  }
}

}  // namespace objc3::io::json
