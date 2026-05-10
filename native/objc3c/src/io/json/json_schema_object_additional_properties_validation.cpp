#include "io/json/json_schema_object_additional_properties_validation.h"

#include "io/json/json_schema_object_additional_properties_keyword_validation.h"
#include "io/json/json_schema_object_additional_properties_schema_validation.h"
#include "io/json/json_schema_object_additional_properties_unexpected_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAdditionalProperties(const JsonValue &schema_root,
                                            const JsonValue &schema,
                                            const JsonValue &payload,
                                            const JsonValue *properties,
                                            const std::string &instance_path,
                                            const std::string &schema_path,
                                            JsonSchemaResult &result) {
  const JsonValue *additional_properties = schema.Find("additionalProperties");
  if (additional_properties != nullptr && payload.IsObject()) {
    if (!ValidateJsonSchemaAdditionalPropertiesKeyword(
            *additional_properties, schema_path, result)) {
      return;
    }
    for (const auto &[key, value] : payload.AsObject()) {
      const bool declared_property =
          properties != nullptr && properties->IsObject() &&
          properties->Find(key) != nullptr;
      if (declared_property) {
        continue;
      }
      if (ValidateJsonSchemaUnexpectedAdditionalProperty(
              *additional_properties, key, instance_path, schema_path,
              result)) {
        continue;
      }
      ValidateJsonSchemaAdditionalPropertySchema(
          schema_root, *additional_properties, value, key, instance_path,
          schema_path, result);
    }
  }
}

}  // namespace objc3::io::json
