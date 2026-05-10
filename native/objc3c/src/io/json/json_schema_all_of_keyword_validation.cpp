#include "io/json/json_schema_all_of_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaAllOfKeywordValidation ValidateJsonSchemaAllOfKeyword(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *all_of = schema.Find("allOf");
  if (all_of == nullptr) {
    return {};
  }
  if (!all_of->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_all_of", JsonSchemaKeywordPath(schema_path, "allOf"),
        "allOf must be an array of schema objects");
    JsonSchemaAllOfKeywordValidation invalid;
    invalid.valid = false;
    return invalid;
  }
  JsonSchemaAllOfKeywordValidation valid;
  valid.value = all_of;
  return valid;
}

}  // namespace objc3::io::json
