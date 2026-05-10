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
  const JsonSchemaArrayContainsKeywordValidation contains =
      ValidateJsonSchemaArrayContainsKeyword(schema, payload, schema_path,
                                             result);
  if (!contains.valid) {
    return false;
  }
  if (contains.value == nullptr) {
    return true;
  }
  ValidateJsonSchemaArrayContainsMatch(schema_root, *contains.value, payload,
                                       instance_path, schema_path, result);
  return true;
}

}  // namespace objc3::io::json
