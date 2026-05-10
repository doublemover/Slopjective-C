#include "io/json/json_schema_numeric_maximum_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaMaximumContract(const JsonValue &schema,
                                       const std::string &schema_path,
                                       JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("maximum");
  if (value != nullptr && !value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, "maximum"),
        "maximum must be a number");
  }
}

}  // namespace objc3::io::json
