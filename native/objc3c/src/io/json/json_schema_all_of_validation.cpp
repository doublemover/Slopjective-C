#include "io/json/json_schema_all_of_validation.h"

#include "io/json/json_schema_all_of_candidate_validation.h"
#include "io/json/json_schema_all_of_keyword_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOf(const JsonValue &schema_root,
                             const JsonValue &schema,
                             const JsonValue &payload,
                             const std::string &instance_path,
                             const std::string &schema_path,
                             JsonSchemaResult &result) {
  const JsonSchemaAllOfKeywordValidation all_of =
      ValidateJsonSchemaAllOfKeyword(schema, schema_path, result);
  if (all_of.value == nullptr) {
    return;
  }
  if (!all_of.valid) {
    return;
  }
  ValidateJsonSchemaAllOfCandidates(schema_root, all_of.value->AsArray(),
                                    payload, instance_path, schema_path,
                                    result);
}

}  // namespace objc3::io::json
