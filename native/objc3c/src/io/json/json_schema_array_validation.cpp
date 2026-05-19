#include "io/json/json_schema_array_validation.h"

#include "io/json/json_schema_array_applicator_dispatch_validation.h"
#include "io/json/json_schema_array_post_constraint_dispatch_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayFields(const JsonValue &schema_root,
                                   const JsonValue &schema,
                                   const JsonValue &payload,
                                   const std::string &instance_path,
                                   const std::string &schema_path,
                                   JsonSchemaResult &result) {
  if (!ValidateJsonSchemaArrayApplicatorDispatch(
          schema_root, schema, payload, instance_path, schema_path, result)) {
    return;
  }
  if (!ValidateJsonSchemaArrayPostConstraintDispatch(
          schema, payload, instance_path, schema_path, result)) {
    return;
  }
}

}  // namespace objc3::io::json
