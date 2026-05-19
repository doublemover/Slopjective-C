#include "io/json/json_schema_any_of_validation.h"

#include "io/json/json_schema_any_of_keyword_validation.h"
#include "io/json/json_schema_any_of_match_validation.h"
#include "io/json/json_schema_any_of_mismatch_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaAnyOf(const JsonValue &schema_root,
                             const JsonValue &schema,
                             const JsonValue &payload,
                             const std::string &instance_path,
                             const std::string &schema_path,
                             JsonSchemaResult &result) {
  const JsonSchemaAnyOfKeywordValidation any_of =
      ValidateJsonSchemaAnyOfKeyword(schema, schema_path, result);
  if (any_of.value == nullptr) {
    return any_of.valid;
  }

  const JsonSchemaAnyOfCandidateMatch match = ValidateJsonSchemaAnyOfMatch(
      schema_root, any_of.value->AsArray(), payload, instance_path, schema_path,
      result);
  ValidateJsonSchemaAnyOfMismatch(match, instance_path, schema_path, result);
  return true;
}

}  // namespace objc3::io::json
