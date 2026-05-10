#include "io/json/json_schema_value_literal_const_validation.h"

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaConstLiteralKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *const_value = schema.Find("const");
  if (const_value != nullptr && !JsonEquals(*const_value, payload)) {
    AddJsonSchemaPayloadError(
        result, "const_mismatch", instance_path,
        JsonSchemaKeywordPath(schema_path, "const"),
        "value did not match const value");
  }
}

}  // namespace objc3::io::json
