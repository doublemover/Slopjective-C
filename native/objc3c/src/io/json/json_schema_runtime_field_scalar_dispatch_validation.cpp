#include "io/json/json_schema_runtime_field_scalar_dispatch_validation.h"

#include "io/json/json_schema_scalar_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRuntimeScalarFields(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaScalarFields(schema, payload, instance_path, schema_path,
                                 result);
}

}  // namespace objc3::io::json
