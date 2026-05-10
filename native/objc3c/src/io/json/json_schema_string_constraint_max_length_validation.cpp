#include "io/json/json_schema_string_constraint_max_length_validation.h"

#include "io/json/json_schema_string_constraint_max_length_keyword_validation.h"
#include "io/json/json_schema_string_constraint_max_length_payload_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringMaxLengthConstraint(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *max_length = ValidateJsonSchemaStringMaxLengthKeyword(
      schema, payload, schema_path, result);
  if (max_length == nullptr) {
    return;
  }
  ValidateJsonSchemaStringMaxLengthPayload(*max_length, payload, instance_path,
                                           schema_path, result);
}

}  // namespace objc3::io::json
