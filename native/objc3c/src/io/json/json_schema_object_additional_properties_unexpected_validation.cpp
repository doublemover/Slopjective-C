#include "io/json/json_schema_object_additional_properties_unexpected_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaUnexpectedAdditionalProperty(
    const JsonValue &additional_properties, const std::string &key,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result) {
  if (additional_properties.IsBool() && !additional_properties.AsBool()) {
    AddJsonSchemaPayloadError(
        result, "unexpected_property",
        JsonInstancePropertyPath(instance_path, key),
        JsonSchemaKeywordPath(schema_path, "additionalProperties"),
        "unexpected property " + key);
    return true;
  }
  return false;
}

}  // namespace objc3::io::json
