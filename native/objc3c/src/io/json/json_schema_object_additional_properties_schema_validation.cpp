#include "io/json/json_schema_object_additional_properties_schema_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAdditionalPropertySchema(
    const JsonValue &schema_root, const JsonValue &additional_properties,
    const JsonValue &value, const std::string &key,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result) {
  if (additional_properties.IsObject()) {
    ValidateJsonSchemaNode(
        schema_root, additional_properties, value,
        JsonInstancePropertyPath(instance_path, key),
        JsonSchemaKeywordPath(schema_path, "additionalProperties"), result);
  }
}

}  // namespace objc3::io::json
