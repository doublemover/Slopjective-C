#include "io/json/json_schema_value_type_match_validation.h"

#include "io/json/json_schema_type.h"
#include "io/json/json_schema_value_type_mismatch_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueTypeMatch(
    const JsonValue &schema_type,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (JsonSchemaMatchesType(schema_type, payload)) {
    return true;
  }
  AddJsonSchemaValueTypeMismatchError(schema_type, payload, instance_path,
                                      schema_path, result);
  return false;
}

}  // namespace objc3::io::json
