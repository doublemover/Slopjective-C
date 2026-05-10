#include "io/json/json_schema_numeric_constraint_maximum_validation.h"

#include "io/json/json_schema_numeric_constraint_maximum_keyword_validation.h"
#include "io/json/json_schema_numeric_constraint_maximum_payload_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaMaximumConstraint(const JsonValue &schema,
                                         const JsonValue &payload,
                                         const std::string &instance_path,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  const JsonValue *maximum = ValidateJsonSchemaMaximumKeyword(
      schema, payload, schema_path, result);
  if (maximum == nullptr) {
    return;
  }
  ValidateJsonSchemaMaximumPayload(*maximum, payload, instance_path,
                                   schema_path, result);
}

}  // namespace objc3::io::json
