#include "io/json/json_schema_numeric_constraint_minimum_payload_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaMinimumPayload(
    const JsonValue &minimum,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (payload.AsNumber() < minimum.AsNumber()) {
    AddJsonSchemaPayloadError(
        result, "minimum", instance_path,
        JsonSchemaKeywordPath(schema_path, "minimum"),
        "number is below minimum");
  }
}

}  // namespace objc3::io::json
