#include "io/json/json_schema_numeric_minimum_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaMinimumContract(const JsonValue &schema,
                                       const std::string &schema_path,
                                       JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("minimum");
  if (value != nullptr && !value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, "minimum"),
        "minimum must be a number");
  }
}

}  // namespace objc3::io::json
