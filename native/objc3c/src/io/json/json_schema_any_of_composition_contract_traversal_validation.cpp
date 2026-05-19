#include "io/json/json_schema_any_of_composition_contract_traversal_validation.h"

#include <cstddef>

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaAnyOfContractTraversal(
    const JsonValue &schema_root,
    const JsonValue::Array &any_of_schemas,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  for (std::size_t i = 0; i < any_of_schemas.size(); ++i) {
    ValidateJsonSchemaNodeContract(
        schema_root, any_of_schemas[i],
        JsonSchemaArrayElementPath(schema_path, "anyOf", i), result);
  }
}

}  // namespace objc3::io::json
