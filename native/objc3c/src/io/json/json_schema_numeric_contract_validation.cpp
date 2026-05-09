#include "io/json/json_schema_numeric_contract_validation.h"

#include <string_view>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaNumberKeyword(const JsonValue &schema,
                                     std::string_view keyword,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *value = schema.Find(keyword);
  if (value != nullptr && !value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, keyword),
        std::string(keyword) + " must be a number");
  }
}

void ValidateJsonSchemaNonnegativeNumberKeyword(const JsonValue &schema,
                                                std::string_view keyword,
                                                const std::string &schema_path,
                                                JsonSchemaResult &result) {
  const JsonValue *value = schema.Find(keyword);
  if (value == nullptr) {
    return;
  }
  ValidateJsonSchemaNumberKeyword(schema, keyword, schema_path, result);
  if (value->IsNumber() && value->AsNumber() < 0.0) {
    AddJsonSchemaContractError(
        result, "invalid_nonnegative_keyword",
        JsonSchemaKeywordPath(schema_path, keyword),
        std::string(keyword) + " must be zero or greater");
  }
}

void ValidateJsonSchemaNumericAssertionContracts(const JsonValue &schema,
                                                 const std::string &schema_path,
                                                 JsonSchemaResult &result) {
  ValidateJsonSchemaNumberKeyword(schema, "minimum", schema_path, result);
  ValidateJsonSchemaNumberKeyword(schema, "maximum", schema_path, result);
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
