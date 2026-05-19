#include "io/json/json_schema_all_of_composition_contract_traversal_validation.h"

#include <cstddef>

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOfContractTraversal(
    const JsonValue &schema_root,
    const JsonValue::Array &all_of_schemas,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  for (std::size_t i = 0; i < all_of_schemas.size(); ++i) {
    ValidateJsonSchemaNodeContract(
        schema_root, all_of_schemas[i],
        JsonSchemaArrayElementPath(schema_path, "allOf", i), result);
  }
}

}  // namespace objc3::io::json
