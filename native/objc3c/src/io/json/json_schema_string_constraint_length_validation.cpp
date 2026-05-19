#include "io/json/json_schema_string_constraint_length_validation.h"

#include "io/json/json_schema_string_constraint_max_length_validation.h"
#include "io/json/json_schema_string_constraint_min_length_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringLengthConstraints(
    const JsonValue &schema, const JsonValue &payload,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaStringMinLengthConstraint(schema, payload, instance_path,
                                              schema_path, result);
  ValidateJsonSchemaStringMaxLengthConstraint(schema, payload, instance_path,
                                              schema_path, result);
}

}  // namespace objc3::io::json
