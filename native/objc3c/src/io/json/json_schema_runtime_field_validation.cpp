#include "io/json/json_schema_runtime_field_validation.h"

#include "io/json/json_schema_array_validation.h"
#include "io/json/json_schema_object_validation.h"
#include "io/json/json_schema_scalar_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRuntimeFields(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *properties = schema.Find("properties");
  ValidateJsonSchemaObjectFields(schema_root, schema, payload, properties,
                                 instance_path, schema_path, result);
  ValidateJsonSchemaArrayFields(schema_root, schema, payload, instance_path,
                                schema_path, result);
  ValidateJsonSchemaScalarFields(schema, payload, instance_path, schema_path,
                                 result);
}

}  // namespace objc3::io::json
