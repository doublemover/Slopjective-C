#include "io/json/json_schema_value_type_validation.h"

#include "io/json/json_schema_type.h"
#include "io/json/json_schema_value_type_mismatch_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueTypeKeyword(const JsonValue &schema,
                                        const JsonValue &payload,
                                        const std::string &instance_path,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  if (const JsonValue *schema_type = schema.Find("type");
      schema_type != nullptr) {
    if (!JsonSchemaMatchesType(*schema_type, payload)) {
      AddJsonSchemaValueTypeMismatchError(*schema_type, payload,
                                          instance_path, schema_path, result);
      return false;
    }
  }

  return true;
}

}  // namespace objc3::io::json
