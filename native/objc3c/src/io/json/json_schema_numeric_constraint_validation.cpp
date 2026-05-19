#include "io/json/json_schema_numeric_constraint_validation.h"

#include "io/json/json_schema_numeric_constraint_maximum_validation.h"
#include "io/json/json_schema_numeric_constraint_minimum_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNumericConstraints(const JsonValue &schema,
                                          const JsonValue &payload,
                                          const std::string &instance_path,
                                          const std::string &schema_path,
                                          JsonSchemaResult &result) {
  ValidateJsonSchemaMinimumConstraint(schema, payload, instance_path,
                                      schema_path, result);
  ValidateJsonSchemaMaximumConstraint(schema, payload, instance_path,
                                      schema_path, result);
}

}  // namespace objc3::io::json
