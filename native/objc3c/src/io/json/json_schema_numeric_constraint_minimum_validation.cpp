#include "io/json/json_schema_numeric_constraint_minimum_validation.h"

#include "io/json/json_schema_numeric_constraint_minimum_keyword_validation.h"
#include "io/json/json_schema_numeric_constraint_minimum_payload_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaMinimumConstraint(const JsonValue &schema,
                                         const JsonValue &payload,
                                         const std::string &instance_path,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  const JsonValue *minimum = ValidateJsonSchemaMinimumKeyword(
      schema, payload, schema_path, result);
  if (minimum == nullptr) {
    return;
  }
  ValidateJsonSchemaMinimumPayload(*minimum, payload, instance_path,
                                   schema_path, result);
}

}  // namespace objc3::io::json
