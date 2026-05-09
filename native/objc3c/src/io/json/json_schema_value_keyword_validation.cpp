#include "io/json/json_schema_value_keyword_validation.h"

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_value_type_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueKeywords(const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  if (!ValidateJsonSchemaValueTypeKeyword(schema, payload, instance_path,
                                          schema_path, result)) {
    return false;
  }

  const JsonValue *const_value = schema.Find("const");
  if (const_value != nullptr && !JsonEquals(*const_value, payload)) {
    AddJsonSchemaPayloadError(
        result, "const_mismatch", instance_path,
        JsonSchemaKeywordPath(schema_path, "const"),
        "value did not match const value");
  }

  const JsonValue *enum_values = schema.Find("enum");
  if (enum_values != nullptr && enum_values->IsArray()) {
    bool matched = false;
    for (const JsonValue &candidate : enum_values->AsArray()) {
      if (JsonEquals(candidate, payload)) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddJsonSchemaPayloadError(
          result, "enum_mismatch", instance_path,
          JsonSchemaKeywordPath(schema_path, "enum"),
          "value did not match enum values");
    }
  }

  return true;
}

}  // namespace objc3::io::json
