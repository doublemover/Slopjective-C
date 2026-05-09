#include "io/json/json_schema_object_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectFields(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const JsonValue &payload,
                                    const JsonValue *properties,
                                    const std::string &instance_path,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (const JsonValue *required = schema.Find("required");
      required != nullptr && payload.IsObject()) {
    if (!required->IsArray()) {
      AddJsonSchemaContractError(
          result, "invalid_required",
          JsonSchemaKeywordPath(schema_path, "required"),
          "required must be an array of property-name strings");
    } else {
      const JsonValue::Array &required_entries = required->AsArray();
      for (std::size_t i = 0; i < required_entries.size(); ++i) {
        const JsonValue &entry = required_entries[i];
        if (!entry.IsString()) {
          AddJsonSchemaContractError(
              result, "invalid_required_entry",
              JsonSchemaArrayElementPath(schema_path, "required", i),
              "required entries must be strings");
          continue;
        }
        if (payload.Find(entry.AsString()) == nullptr) {
          AddJsonSchemaPayloadError(
              result, "missing_required",
              JsonInstancePropertyPath(instance_path, entry.AsString()),
              JsonSchemaArrayElementPath(schema_path, "required", i),
              "missing required property " + entry.AsString());
        }
      }
    }
  }
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
