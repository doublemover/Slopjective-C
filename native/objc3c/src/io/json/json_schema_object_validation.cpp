#include "io/json/json_schema_object_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectFields(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const JsonValue &payload,
                                    const JsonValue *properties,
                                    const std::string &path,
                                    JsonSchemaResult &result) {
  if (const JsonValue *required = schema.Find("required");
      required != nullptr && required->IsArray() && payload.IsObject()) {
    for (const JsonValue &entry : required->AsArray()) {
      if (!entry.IsString()) {
        continue;
      }
      if (payload.Find(entry.AsString()) == nullptr) {
        AddJsonSchemaError(
            result, path + " missing required property " + entry.AsString());
      }
    }
  }
  if (properties != nullptr && properties->IsObject() && payload.IsObject()) {
    for (const auto &[key, property_schema] : properties->AsObject()) {
      const JsonValue *property = payload.Find(key);
      if (property != nullptr) {
        ValidateJsonSchemaNode(schema_root, property_schema, *property,
                               path + "." + key, result);
      }
    }
  }
  const JsonValue *additional_properties = schema.Find("additionalProperties");
  if (additional_properties != nullptr && payload.IsObject()) {
    for (const auto &[key, value] : payload.AsObject()) {
      const bool declared_property =
          properties != nullptr && properties->IsObject() &&
          properties->Find(key) != nullptr;
      if (declared_property) {
        continue;
      }
      if (additional_properties->IsBool() && !additional_properties->AsBool()) {
        AddJsonSchemaError(result, path + " unexpected property " + key);
        continue;
      }
      if (additional_properties->IsObject()) {
        ValidateJsonSchemaNode(schema_root, *additional_properties, value,
                               path + "." + key, result);
      }
    }
  }
}

}  // namespace objc3::io::json

