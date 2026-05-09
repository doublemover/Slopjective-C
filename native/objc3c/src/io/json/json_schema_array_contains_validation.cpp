#include "io/json/json_schema_array_contains_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_subschema.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayContains(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *contains = schema.Find("contains");
  if (contains != nullptr && payload.IsArray()) {
    if (!contains->IsObject()) {
      AddJsonSchemaContractError(
          result, "invalid_contains",
          JsonSchemaKeywordPath(schema_path, "contains"),
          "contains must be a schema object");
      return false;
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
  return true;
}

}  // namespace objc3::io::json
