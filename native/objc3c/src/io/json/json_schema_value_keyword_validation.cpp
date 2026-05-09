#include "io/json/json_schema_value_keyword_validation.h"

#include "io/json/json_schema_value_literal_validation.h"
#include "io/json/json_schema_value_type_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueKeywords(const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  if (!ValidateJsonSchemaValueTypeKeyword(schema, payload, instance_path,
                                          schema_path, result)) {
    return false;
  }

  ValidateJsonSchemaValueLiteralKeywords(schema, payload, instance_path,
                                         schema_path, result);

  return true;
}

}  // namespace objc3::io::json
