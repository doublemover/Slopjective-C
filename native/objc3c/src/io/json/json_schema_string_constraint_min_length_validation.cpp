#include "io/json/json_schema_string_constraint_min_length_validation.h"

#include "io/json/json_schema_string_constraint_min_length_keyword_validation.h"
#include "io/json/json_schema_string_constraint_min_length_payload_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringMinLengthConstraint(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *min_length = ValidateJsonSchemaStringMinLengthKeyword(
      schema, payload, schema_path, result);
  if (min_length == nullptr) {
    return;
  }
  ValidateJsonSchemaStringMinLengthPayload(*min_length, payload, instance_path,
                                           schema_path, result);
}

}  // namespace objc3::io::json
