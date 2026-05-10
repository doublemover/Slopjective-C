#include "io/json/json_schema_numeric_minimum_maximum_contract_validation.h"

#include <string>
#include <string_view>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

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

}  // namespace

void ValidateJsonSchemaMinimumMaximumContracts(const JsonValue &schema,
                                               const std::string &schema_path,
                                               JsonSchemaResult &result) {
  ValidateJsonSchemaNumberKeyword(schema, "minimum", schema_path, result);
  ValidateJsonSchemaNumberKeyword(schema, "maximum", schema_path, result);
}

}  // namespace objc3::io::json
