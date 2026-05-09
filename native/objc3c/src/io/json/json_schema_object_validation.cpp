#include "io/json/json_schema_object_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_object_required_validation.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectFields(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const JsonValue &payload,
                                    const JsonValue *properties,
                                    const std::string &instance_path,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  ValidateJsonSchemaRequiredProperties(schema, payload, instance_path,
                                       schema_path, result);
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
  const JsonValue *additional_properties = schema.Find("additionalProperties");
  if (additional_properties != nullptr && payload.IsObject()) {
    if (!additional_properties->IsBool() &&
        !additional_properties->IsObject()) {
      AddJsonSchemaContractError(
          result, "invalid_additional_properties",
          JsonSchemaKeywordPath(schema_path, "additionalProperties"),
          "additionalProperties must be false, true, or a schema object");
      return;
    }
    for (const auto &[key, value] : payload.AsObject()) {
      const bool declared_property =
          properties != nullptr && properties->IsObject() &&
          properties->Find(key) != nullptr;
      if (declared_property) {
        continue;
      }
      if (additional_properties->IsBool() && !additional_properties->AsBool()) {
        AddJsonSchemaPayloadError(
            result, "unexpected_property",
            JsonInstancePropertyPath(instance_path, key),
            JsonSchemaKeywordPath(schema_path, "additionalProperties"),
            "unexpected property " + key);
        continue;
      }
      if (additional_properties->IsObject()) {
        ValidateJsonSchemaNode(schema_root, *additional_properties, value,
                               JsonInstancePropertyPath(instance_path, key),
                               JsonSchemaKeywordPath(schema_path,
                                                     "additionalProperties"),
                               result);
      }
    }
  }
}

}  // namespace objc3::io::json
