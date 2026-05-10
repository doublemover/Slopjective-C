#include "io/json/json_schema_array_cardinality_max_items_payload_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayMaxItemsPayload(
    const JsonValue &max_items,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (payload.AsArray().size() >
      static_cast<std::size_t>(max_items.AsNumber())) {
    AddJsonSchemaPayloadError(
        result, "max_items", instance_path,
        JsonSchemaKeywordPath(schema_path, "maxItems"),
        "array has too many items");
  }
}

}  // namespace objc3::io::json
