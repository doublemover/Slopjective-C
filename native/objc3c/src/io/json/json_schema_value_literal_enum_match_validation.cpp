#include "io/json/json_schema_value_literal_enum_match_validation.h"

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaEnumLiteralMatch(
    const JsonValue &enum_values,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  bool matched = false;
  for (const JsonValue &candidate : enum_values.AsArray()) {
    if (JsonEquals(candidate, payload)) {
      matched = true;
      break;
    }
  }
  if (!matched) {
    AddJsonSchemaPayloadError(
        result, "enum_mismatch", instance_path,
        JsonSchemaKeywordPath(schema_path, "enum"),
        "value did not match enum values");
  }
}

}  // namespace objc3::io::json
