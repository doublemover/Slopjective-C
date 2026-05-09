#include "io/json/json_schema_pattern_contract_validation.h"

#include <regex>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaPatternContract(const JsonValue &schema,
                                       const std::string &schema_path,
                                       JsonSchemaResult &result) {
  if (const JsonValue *pattern = schema.Find("pattern"); pattern != nullptr) {
    if (!pattern->IsString()) {
      AddJsonSchemaContractError(
          result, "invalid_pattern",
          JsonSchemaKeywordPath(schema_path, "pattern"),
          "pattern must be a string");
    } else {
      try {
        (void)std::regex(pattern->AsString());
      } catch (const std::regex_error &) {
        AddJsonSchemaContractError(
            result, "invalid_pattern",
            JsonSchemaKeywordPath(schema_path, "pattern"),
            "pattern is not a valid regular expression");
      }
    }
  }
}

}  // namespace objc3::io::json
