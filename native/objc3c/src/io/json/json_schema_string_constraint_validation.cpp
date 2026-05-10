#include "io/json/json_schema_string_constraint_validation.h"

#include "io/json/json_schema_string_constraint_length_validation.h"
#include "io/json/json_schema_string_constraint_pattern_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaStringConstraints(const JsonValue &schema,
                                         const JsonValue &payload,
                                         const std::string &instance_path,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  ValidateJsonSchemaStringLengthConstraints(schema, payload, instance_path,
                                            schema_path, result);
  ValidateJsonSchemaStringPatternConstraint(schema, payload, instance_path,
                                            schema_path, result);
}

}  // namespace objc3::io::json
