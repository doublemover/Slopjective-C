#include "io/json/json_schema_array_applicator_dispatch_validation.h"

#include "io/json/json_schema_array_contains_validation.h"
#include "io/json/json_schema_array_items_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayApplicatorDispatch(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!ValidateJsonSchemaArrayItems(schema_root, schema, payload, instance_path,
                                    schema_path, result)) {
    return false;
  }
  if (!ValidateJsonSchemaArrayContains(schema_root, schema, payload,
                                       instance_path, schema_path, result)) {
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
