#include "io/json/json_schema_string_constraint_min_length_payload_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringMinLengthPayload(
    const JsonValue &min_length,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (payload.AsString().size() <
      static_cast<std::size_t>(min_length.AsNumber())) {
    AddJsonSchemaPayloadError(
        result, "min_length", instance_path,
        JsonSchemaKeywordPath(schema_path, "minLength"),
        "string is shorter than minLength");
  }
}

}  // namespace objc3::io::json
