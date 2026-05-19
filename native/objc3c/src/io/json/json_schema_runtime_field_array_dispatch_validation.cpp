#include "io/json/json_schema_runtime_field_array_dispatch_validation.h"

#include "io/json/json_schema_array_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRuntimeArrayFields(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaArrayFields(schema_root, schema, payload, instance_path,
                                schema_path, result);
}

}  // namespace objc3::io::json
