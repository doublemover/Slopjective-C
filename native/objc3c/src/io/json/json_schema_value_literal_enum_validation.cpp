#include "io/json/json_schema_value_literal_enum_validation.h"

#include "io/json/json_schema_value_literal_enum_keyword_validation.h"
#include "io/json/json_schema_value_literal_enum_match_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaEnumLiteralKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *enum_values = ValidateJsonSchemaEnumLiteralKeywordArray(
      schema);
  if (enum_values == nullptr) {
    return;
  }
  ValidateJsonSchemaEnumLiteralMatch(*enum_values, payload, instance_path,
                                     schema_path, result);
}

}  // namespace objc3::io::json
