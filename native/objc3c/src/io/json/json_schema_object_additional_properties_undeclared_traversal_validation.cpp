#include "io/json/json_schema_object_additional_properties_undeclared_traversal_validation.h"

#include "io/json/json_schema_object_additional_properties_schema_delegation_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAdditionalPropertiesUndeclaredTraversal(
    const JsonValue &schema_root,
    const JsonValue &additional_properties,
    const JsonValue &payload,
    const JsonValue *properties,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  for (const auto &[key, value] : payload.AsObject()) {
    const bool declared_property =
        properties != nullptr && properties->IsObject() &&
        properties->Find(key) != nullptr;
    if (declared_property) {
      continue;
    }
    ValidateJsonSchemaAdditionalPropertySchemaDelegation(
        schema_root, additional_properties, value, key, instance_path,
        schema_path, result);
  }
}

}  // namespace objc3::io::json
