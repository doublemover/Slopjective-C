#include "io/json/json_schema_array_validation.h"

#include <cstddef>

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_array_items_validation.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_subschema.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayFields(const JsonValue &schema_root,
                                   const JsonValue &schema,
                                   const JsonValue &payload,
                                   const std::string &instance_path,
                                   const std::string &schema_path,
                                   JsonSchemaResult &result) {
  if (!ValidateJsonSchemaArrayItems(schema_root, schema, payload, instance_path,
                                    schema_path, result)) {
    return;
  }
  const JsonValue *contains = schema.Find("contains");
  if (contains != nullptr && payload.IsArray()) {
    if (!contains->IsObject()) {
      AddJsonSchemaContractError(
          result, "invalid_contains",
          JsonSchemaKeywordPath(schema_path, "contains"),
          "contains must be a schema object");
      return;
    }
    bool matched = false;
    for (const JsonValue &item : payload.AsArray()) {
      if (JsonSubschemaPasses(schema_root, *contains, item,
                              instance_path + "[]",
                              JsonSchemaKeywordPath(schema_path, "contains"))) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddJsonSchemaPayloadError(
          result, "contains", instance_path,
          JsonSchemaKeywordPath(schema_path, "contains"),
          "array did not contain a matching item");
    }
  }
  const JsonValue *min_items = schema.Find("minItems");
  if (min_items != nullptr && payload.IsArray()) {
    if (!min_items->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_min_items",
          JsonSchemaKeywordPath(schema_path, "minItems"),
          "minItems must be a number");
    } else if (payload.AsArray().size() <
               static_cast<std::size_t>(min_items->AsNumber())) {
      AddJsonSchemaPayloadError(
          result, "min_items", instance_path,
          JsonSchemaKeywordPath(schema_path, "minItems"),
          "array has too few items");
    }
  }
  const JsonValue *max_items = schema.Find("maxItems");
  if (max_items != nullptr && payload.IsArray()) {
    if (!max_items->IsNumber()) {
      AddJsonSchemaContractError(
          result, "invalid_max_items",
          JsonSchemaKeywordPath(schema_path, "maxItems"),
          "maxItems must be a number");
    } else if (payload.AsArray().size() >
               static_cast<std::size_t>(max_items->AsNumber())) {
      AddJsonSchemaPayloadError(
          result, "max_items", instance_path,
          JsonSchemaKeywordPath(schema_path, "maxItems"),
          "array has too many items");
    }
  }
  const JsonValue *unique_items = schema.Find("uniqueItems");
  if (unique_items != nullptr && payload.IsArray()) {
    if (!unique_items->IsBool()) {
      AddJsonSchemaContractError(
          result, "invalid_unique_items",
          JsonSchemaKeywordPath(schema_path, "uniqueItems"),
          "uniqueItems must be a boolean");
      return;
    }
    if (!unique_items->AsBool()) {
      return;
    }
    const JsonValue::Array &array = payload.AsArray();
    bool duplicate = false;
    for (std::size_t i = 0; i < array.size(); ++i) {
      for (std::size_t j = i + 1; j < array.size(); ++j) {
        if (JsonEquals(array[i], array[j])) {
          duplicate = true;
          break;
        }
      }
      if (duplicate) {
        AddJsonSchemaPayloadError(
            result, "unique_items", instance_path,
            JsonSchemaKeywordPath(schema_path, "uniqueItems"),
            "array contains duplicate items");
        break;
      }
    }
  }
}

}  // namespace objc3::io::json
