#include "io/json/json_schema_any_of_composition_contract_array_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

const JsonValue *ValidateJsonSchemaAnyOfContractArray(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *any_of = schema.Find("anyOf");
  if (any_of == nullptr) {
    return nullptr;
  }
  if (!any_of->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_schema_array",
        JsonSchemaKeywordPath(schema_path, "anyOf"),
        "anyOf must be an array of schema objects");
    return nullptr;
  }
  if (any_of->AsArray().empty()) {
    AddJsonSchemaContractError(
        result, "empty_schema_array",
        JsonSchemaKeywordPath(schema_path, "anyOf"),
        "anyOf must contain at least one schema");
  }
  return any_of;
}

}  // namespace objc3::io::json
