#include "io/json/json_schema_pattern_keyword_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaPatternKeywordContract(const JsonValue &pattern,
                                              const std::string &schema_path,
                                              JsonSchemaResult &result) {
  if (!pattern.IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_pattern",
        JsonSchemaKeywordPath(schema_path, "pattern"),
        "pattern must be a string");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
