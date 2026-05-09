#include "io/json/json_schema_any_of_composition_contract_validation.h"

#include <cstddef>

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaAnyOfContract(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *any_of = schema.Find("anyOf");
  if (any_of == nullptr) {
    return;
  }
  if (!any_of->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_schema_array",
        JsonSchemaKeywordPath(schema_path, "anyOf"),
        "anyOf must be an array of schema objects");
    return;
  }
  if (any_of->AsArray().empty()) {
    AddJsonSchemaContractError(
        result, "empty_schema_array",
        JsonSchemaKeywordPath(schema_path, "anyOf"),
        "anyOf must contain at least one schema");
  }
  for (std::size_t i = 0; i < any_of->AsArray().size(); ++i) {
    ValidateJsonSchemaNodeContract(
        schema_root, any_of->AsArray()[i],
        JsonSchemaArrayElementPath(schema_path, "anyOf", i), result);
  }
}

}  // namespace objc3::io::json
