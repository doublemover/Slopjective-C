#include "io/json/json_schema_value_literal_validation.h"

#include "io/json/json_schema_value_literal_const_validation.h"
#include "io/json/json_schema_value_literal_enum_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaValueLiteralKeywords(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaConstLiteralKeyword(schema, payload, instance_path,
                                        schema_path, result);
  ValidateJsonSchemaEnumLiteralKeyword(schema, payload, instance_path,
                                       schema_path, result);
}

}  // namespace objc3::io::json
