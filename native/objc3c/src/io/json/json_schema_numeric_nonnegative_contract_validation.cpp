#include "io/json/json_schema_numeric_nonnegative_contract_validation.h"

#include <string>
#include <string_view>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

void ValidateJsonSchemaNonnegativeNumberKeyword(
    const JsonValue &schema, std::string_view keyword,
    const std::string &schema_path, JsonSchemaResult &result) {
  const JsonValue *value = schema.Find(keyword);
  if (value == nullptr) {
    return;
  }
  if (!value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, keyword),
        std::string(keyword) + " must be a number");
  }
  if (value->IsNumber() && value->AsNumber() < 0.0) {
    AddJsonSchemaContractError(
        result, "invalid_nonnegative_keyword",
        JsonSchemaKeywordPath(schema_path, keyword),
        std::string(keyword) + " must be zero or greater");
  }
}

}  // namespace

void ValidateJsonSchemaNonnegativeNumericKeywordContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "minLength", schema_path,
                                             result);
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "maxLength", schema_path,
                                             result);
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "minItems", schema_path,
                                             result);
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "maxItems", schema_path,
                                             result);
}

}  // namespace objc3::io::json
