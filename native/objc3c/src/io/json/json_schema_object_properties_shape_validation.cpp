#include "io/json/json_schema_object_properties_shape_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaObjectPropertiesShape(
    const JsonValue *properties,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (properties == nullptr) {
    return nullptr;
  }
  if (!properties->IsObject()) {
    AddJsonSchemaContractError(
        result, "invalid_properties",
        JsonSchemaKeywordPath(schema_path, "properties"),
        "properties must be an object of property schemas");
    return nullptr;
  }
  return properties;
}

}  // namespace objc3::io::json
