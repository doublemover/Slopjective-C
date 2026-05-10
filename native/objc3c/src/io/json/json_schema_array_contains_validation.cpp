#include "io/json/json_schema_array_contains_validation.h"

#include "io/json/json_schema_array_contains_keyword_validation.h"
#include "io/json/json_schema_array_contains_match_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayContains(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *contains = schema.Find("contains");
  if (contains != nullptr && payload.IsArray()) {
    if (!ValidateJsonSchemaArrayContainsKeyword(*contains, schema_path,
                                                result)) {
      return false;
    }
    ValidateJsonSchemaArrayContainsMatch(schema_root, *contains, payload,
                                         instance_path, schema_path, result);
  }
  return true;
}

}  // namespace objc3::io::json
