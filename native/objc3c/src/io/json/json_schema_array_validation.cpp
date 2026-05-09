#include "io/json/json_schema_array_validation.h"

#include <cstddef>

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_subschema.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayFields(const JsonValue &schema_root,
                                   const JsonValue &schema,
                                   const JsonValue &payload,
                                   const std::string &path,
                                   JsonSchemaResult &result) {
  const JsonValue *items = schema.Find("items");
  if (items != nullptr && payload.IsArray()) {
    const auto &array = payload.AsArray();
    for (std::size_t i = 0; i < array.size(); ++i) {
      ValidateJsonSchemaNode(schema_root, *items, array[i],
                             path + "[" + std::to_string(i) + "]", result);
    }
  }
  const JsonValue *contains = schema.Find("contains");
  if (contains != nullptr && payload.IsArray()) {
    bool matched = false;
    for (const JsonValue &item : payload.AsArray()) {
      if (JsonSubschemaPasses(schema_root, *contains, item, path + "[]")) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddJsonSchemaError(result, path + " did not contain a matching item");
    }
  }
  const JsonValue *min_items = schema.Find("minItems");
  if (min_items != nullptr && min_items->IsNumber() && payload.IsArray() &&
      payload.AsArray().size() <
          static_cast<std::size_t>(min_items->AsNumber())) {
    AddJsonSchemaError(result, path + " has too few items");
  }
  const JsonValue *max_items = schema.Find("maxItems");
  if (max_items != nullptr && max_items->IsNumber() && payload.IsArray() &&
      payload.AsArray().size() >
          static_cast<std::size_t>(max_items->AsNumber())) {
    AddJsonSchemaError(result, path + " has too many items");
  }
  const JsonValue *unique_items = schema.Find("uniqueItems");
  if (unique_items != nullptr && unique_items->IsBool() &&
      unique_items->AsBool() && payload.IsArray()) {
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
        AddJsonSchemaError(result, path + " contains duplicate items");
        break;
      }
    }
  }
}

}  // namespace objc3::io::json

