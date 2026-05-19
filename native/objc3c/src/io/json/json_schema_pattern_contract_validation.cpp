#include "io/json/json_schema_pattern_contract_validation.h"

#include "io/json/json_schema_pattern_keyword_contract_validation.h"
#include "io/json/json_schema_pattern_regex_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPatternContract(const JsonValue &schema,
                                       const std::string &schema_path,
                                       JsonSchemaResult &result) {
  if (const JsonValue *pattern = schema.Find("pattern"); pattern != nullptr) {
    if (!ValidateJsonSchemaPatternKeywordContract(*pattern, schema_path,
                                                  result)) {
      return;
    }
    ValidateJsonSchemaPatternRegexContract(*pattern, schema_path, result);
  }
}

}  // namespace objc3::io::json
