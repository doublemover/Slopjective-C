#include "io/json/json_schema_numeric_constraint_maximum_payload_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaMaximumPayload(
    const JsonValue &maximum,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (payload.AsNumber() > maximum.AsNumber()) {
    AddJsonSchemaPayloadError(
        result, "maximum", instance_path,
        JsonSchemaKeywordPath(schema_path, "maximum"),
        "number is above maximum");
  }
}

}  // namespace objc3::io::json
