#include "io/json/json_schema_object_properties_validation.h"

#include "io/json/json_schema_object_properties_shape_validation.h"
#include "io/json/json_schema_object_properties_traversal_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaDeclaredProperties(const JsonValue &schema_root,
                                          const JsonValue &payload,
                                          const JsonValue *properties,
                                          const std::string &instance_path,
                                          const std::string &schema_path,
                                          JsonSchemaResult &result) {
  const JsonValue *declared_properties =
      ValidateJsonSchemaObjectPropertiesShape(properties, schema_path, result);
  if (declared_properties == nullptr) {
    return;
  }
  ValidateJsonSchemaDeclaredPropertyTraversal(
      schema_root, payload, *declared_properties, instance_path, schema_path,
      result);
}

}  // namespace objc3::io::json
