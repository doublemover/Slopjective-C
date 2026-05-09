#include "io/json/json_schema_object_validation.h"

#include "io/json/json_schema_object_additional_properties_validation.h"
#include "io/json/json_schema_object_properties_validation.h"
#include "io/json/json_schema_object_required_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectFields(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const JsonValue &payload,
                                    const JsonValue *properties,
                                    const std::string &instance_path,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  ValidateJsonSchemaRequiredProperties(schema, payload, instance_path,
                                       schema_path, result);
  ValidateJsonSchemaDeclaredProperties(schema_root, payload, properties,
                                       instance_path, schema_path, result);
  ValidateJsonSchemaAdditionalProperties(schema_root, schema, payload,
                                         properties, instance_path,
                                         schema_path, result);
}

}  // namespace objc3::io::json
