#include "io/json/json_schema_value_literal_enum_keyword_validation.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaEnumLiteralKeywordArray(
    const JsonValue &schema) {
  const JsonValue *enum_values = schema.Find("enum");
  if (enum_values == nullptr || !enum_values->IsArray()) {
    return nullptr;
  }
  return enum_values;
}

}  // namespace objc3::io::json
