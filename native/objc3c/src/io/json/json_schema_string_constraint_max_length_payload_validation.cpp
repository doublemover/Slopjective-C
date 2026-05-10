#include "io/json/json_schema_string_constraint_max_length_payload_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringMaxLengthPayload(
    const JsonValue &max_length,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (payload.AsString().size() >
      static_cast<std::size_t>(max_length.AsNumber())) {
    AddJsonSchemaPayloadError(
        result, "max_length", instance_path,
        JsonSchemaKeywordPath(schema_path, "maxLength"),
        "string is longer than maxLength");
  }
}

}  // namespace objc3::io::json
