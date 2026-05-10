#include "io/json/json_schema_ref_preflight_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaRefPreflightKeywordValidation ValidateJsonSchemaRefPreflightKeyword(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *ref = schema.Find("$ref");
  if (ref == nullptr) {
    JsonSchemaRefPreflightKeywordValidation missing;
    missing.continue_current_schema = true;
    return missing;
  }
  if (!ref->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
        "$ref must be a local JSON pointer string");
    JsonSchemaRefPreflightKeywordValidation invalid;
    invalid.valid = false;
    invalid.ref_keyword = ref;
    return invalid;
  }

  JsonSchemaRefPreflightKeywordValidation valid;
  valid.ref_keyword = ref;
  return valid;
}

}  // namespace objc3::io::json
