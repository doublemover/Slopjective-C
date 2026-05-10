#include "io/json/json_schema_numeric_nonnegative_string_length_min_length_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaNonnegativeMinLengthContract(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("minLength");
  if (value == nullptr) {
    return;
  }
  if (!value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, "minLength"),
        "minLength must be a number");
  }
  if (value->IsNumber() && value->AsNumber() < 0.0) {
    AddJsonSchemaContractError(
        result, "invalid_nonnegative_keyword",
        JsonSchemaKeywordPath(schema_path, "minLength"),
        "minLength must be zero or greater");
  }
}

}  // namespace objc3::io::json
