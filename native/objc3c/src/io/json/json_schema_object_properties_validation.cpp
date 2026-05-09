#include "io/json/json_schema_object_properties_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaDeclaredProperties(const JsonValue &schema_root,
                                          const JsonValue &payload,
                                          const JsonValue *properties,
                                          const std::string &instance_path,
                                          const std::string &schema_path,
                                          JsonSchemaResult &result) {
  if (properties != nullptr) {
    if (!properties->IsObject()) {
      AddJsonSchemaContractError(
          result, "invalid_properties",
          JsonSchemaKeywordPath(schema_path, "properties"),
          "properties must be an object of property schemas");
    } else if (payload.IsObject()) {
      for (const auto &[key, property_schema] : properties->AsObject()) {
        const JsonValue *property = payload.Find(key);
        if (property != nullptr) {
          ValidateJsonSchemaNode(schema_root, property_schema, *property,
                                 JsonInstancePropertyPath(instance_path, key),
                                 JsonSchemaPropertySchemaPath(schema_path, key),
                                 result);
        }
      }
    }
  }
}

}  // namespace objc3::io::json
