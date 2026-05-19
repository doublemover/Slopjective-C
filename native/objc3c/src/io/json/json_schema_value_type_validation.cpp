#include "io/json/json_schema_value_type_validation.h"

#include "io/json/json_schema_value_type_keyword_validation.h"
#include "io/json/json_schema_value_type_match_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueTypeKeyword(const JsonValue &schema,
                                        const JsonValue &payload,
                                        const std::string &instance_path,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  const JsonValue *schema_type = FindJsonSchemaValueTypeKeyword(schema);
  if (schema_type == nullptr) {
    return true;
  }
  return ValidateJsonSchemaValueTypeMatch(*schema_type, payload, instance_path,
                                          schema_path, result);
}

}  // namespace objc3::io::json
