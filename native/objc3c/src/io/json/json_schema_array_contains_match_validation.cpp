#include "io/json/json_schema_array_contains_match_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_subschema.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayContainsMatch(
    const JsonValue &schema_root,
    const JsonValue &contains,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  bool matched = false;
  for (const JsonValue &item : payload.AsArray()) {
    if (JsonSubschemaPasses(schema_root, contains, item, instance_path + "[]",
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

}  // namespace objc3::io::json
