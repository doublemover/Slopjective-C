#include "io/json/json_schema_all_of_composition_contract_array_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaAllOfContractArray(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *all_of = schema.Find("allOf");
  if (all_of == nullptr) {
    return nullptr;
  }
  if (!all_of->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_schema_array",
        JsonSchemaKeywordPath(schema_path, "allOf"),
        "allOf must be an array of schema objects");
    return nullptr;
  }
  if (all_of->AsArray().empty()) {
    AddJsonSchemaContractError(
        result, "empty_schema_array",
        JsonSchemaKeywordPath(schema_path, "allOf"),
        "allOf must contain at least one schema");
  }
  return all_of;
}

}  // namespace objc3::io::json
