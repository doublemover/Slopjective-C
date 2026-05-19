#include "io/json/json_schema_any_of_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaAnyOfKeywordValidation ValidateJsonSchemaAnyOfKeyword(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *any_of = schema.Find("anyOf");
  if (any_of == nullptr) {
    return {};
  }
  if (!any_of->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_any_of", JsonSchemaKeywordPath(schema_path, "anyOf"),
        "anyOf must be an array of schema objects");
    JsonSchemaAnyOfKeywordValidation invalid;
    invalid.valid = false;
    return invalid;
  }
  JsonSchemaAnyOfKeywordValidation valid;
  valid.value = any_of;
  return valid;
}

}  // namespace objc3::io::json
