#include "io/json/json_schema_value_literal_const_validation.h"

#include "io/json/json_schema_value_literal_const_keyword_validation.h"
#include "io/json/json_schema_value_literal_const_match_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaConstLiteralKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *const_value = FindJsonSchemaConstLiteralKeyword(schema);
  if (const_value == nullptr) {
    return;
  }
  ValidateJsonSchemaConstLiteralMatch(*const_value, payload, instance_path,
                                      schema_path, result);
}

}  // namespace objc3::io::json
