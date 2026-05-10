#include "io/json/json_schema_array_contains_keyword_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayContainsKeyword(
    const JsonValue &contains,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!contains.IsObject()) {
    AddJsonSchemaContractError(
        result, "invalid_contains",
        JsonSchemaKeywordPath(schema_path, "contains"),
        "contains must be a schema object");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
