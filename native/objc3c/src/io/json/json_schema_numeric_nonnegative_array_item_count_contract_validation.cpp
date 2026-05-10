#include "io/json/json_schema_numeric_nonnegative_array_item_count_contract_validation.h"

#include <string>
#include <string_view>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

void ValidateJsonSchemaNonnegativeArrayItemCountKeyword(
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

void ValidateJsonSchemaNonnegativeArrayItemCountKeywordContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaNonnegativeArrayItemCountKeyword(
      schema, "minItems", schema_path, result);
  ValidateJsonSchemaNonnegativeArrayItemCountKeyword(
      schema, "maxItems", schema_path, result);
}

}  // namespace objc3::io::json
