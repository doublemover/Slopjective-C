#include "io/json/json_schema_runtime_field_validation.h"

#include "io/json/json_schema_runtime_field_array_dispatch_validation.h"
#include "io/json/json_schema_runtime_field_object_dispatch_validation.h"
#include "io/json/json_schema_runtime_field_scalar_dispatch_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRuntimeFields(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  ValidateJsonSchemaRuntimeObjectFields(schema_root, schema, payload,
                                        instance_path, schema_path, result);
  ValidateJsonSchemaRuntimeArrayFields(schema_root, schema, payload,
                                       instance_path, schema_path, result);
  ValidateJsonSchemaRuntimeScalarFields(schema, payload, instance_path,
                                        schema_path, result);
}

}  // namespace objc3::io::json
