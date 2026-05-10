#include "io/json/json_schema_object_required_missing_property_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectMissingRequiredProperty(
    const JsonValue &payload,
    const std::string &entry_name,
    std::size_t entry_index,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (payload.Find(entry_name) == nullptr) {
    AddJsonSchemaPayloadError(
        result, "missing_required",
        JsonInstancePropertyPath(instance_path, entry_name),
        JsonSchemaArrayElementPath(schema_path, "required", entry_index),
        "missing required property " + entry_name);
  }
}

}  // namespace objc3::io::json
