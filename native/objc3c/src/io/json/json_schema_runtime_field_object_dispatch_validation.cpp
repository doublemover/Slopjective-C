#include "io/json/json_schema_runtime_field_object_dispatch_validation.h"

#include "io/json/json_schema_object_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRuntimeObjectFields(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *properties = schema.Find("properties");
  ValidateJsonSchemaObjectFields(schema_root, schema, payload, properties,
                                 instance_path, schema_path, result);
}

}  // namespace objc3::io::json
