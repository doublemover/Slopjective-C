#include "io/json/json_schema_all_of_composition_contract_validation.h"

#include <cstddef>

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOfContract(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *all_of = schema.Find("allOf");
  if (all_of == nullptr) {
    return;
  }
  if (!all_of->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_schema_array",
        JsonSchemaKeywordPath(schema_path, "allOf"),
        "allOf must be an array of schema objects");
    return;
  }
  if (all_of->AsArray().empty()) {
    AddJsonSchemaContractError(
        result, "empty_schema_array",
        JsonSchemaKeywordPath(schema_path, "allOf"),
        "allOf must contain at least one schema");
  }
  for (std::size_t i = 0; i < all_of->AsArray().size(); ++i) {
    ValidateJsonSchemaNodeContract(
        schema_root, all_of->AsArray()[i],
        JsonSchemaArrayElementPath(schema_path, "allOf", i), result);
  }
}

}  // namespace objc3::io::json
