#include "io/json/json_schema_object_additional_properties_schema_delegation_validation.h"

#include "io/json/json_schema_object_additional_properties_schema_validation.h"
#include "io/json/json_schema_object_additional_properties_unexpected_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAdditionalPropertySchemaDelegation(
    const JsonValue &schema_root,
    const JsonValue &additional_properties,
    const JsonValue &value,
    const std::string &key,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (ValidateJsonSchemaUnexpectedAdditionalProperty(
          additional_properties, key, instance_path, schema_path, result)) {
    return;
  }
  ValidateJsonSchemaAdditionalPropertySchema(
      schema_root, additional_properties, value, key, instance_path,
      schema_path, result);
}

}  // namespace objc3::io::json
