#include "io/json/json_schema_array_post_constraint_dispatch_validation.h"

#include "io/json/json_schema_array_cardinality_validation.h"
#include "io/json/json_schema_array_uniqueness_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayPostConstraintDispatch(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaArrayCardinality(schema, payload, instance_path,
                                     schema_path, result);
  if (!ValidateJsonSchemaArrayUniqueness(schema, payload, instance_path,
                                         schema_path, result)) {
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
