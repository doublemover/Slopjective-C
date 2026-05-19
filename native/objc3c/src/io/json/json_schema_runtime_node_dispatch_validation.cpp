#include "io/json/json_schema_runtime_node_dispatch_validation.h"

#include "io/json/json_schema_runtime_field_validation.h"
#include "io/json/json_schema_value_keyword_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaRuntimeNodeDispatch(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!ValidateJsonSchemaValueKeywords(schema, payload, instance_path,
                                       schema_path, result)) {
    return false;
  }
  ValidateJsonSchemaRuntimeFields(schema_root, schema, payload, instance_path,
                                  schema_path, result);
  return true;
}

}  // namespace objc3::io::json
