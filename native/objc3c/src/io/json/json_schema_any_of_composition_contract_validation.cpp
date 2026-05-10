#include "io/json/json_schema_any_of_composition_contract_validation.h"

#include "io/json/json_schema_any_of_composition_contract_array_validation.h"
#include "io/json/json_schema_any_of_composition_contract_traversal_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAnyOfContract(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *any_of =
      ValidateJsonSchemaAnyOfContractArray(schema, schema_path, result);
  if (any_of == nullptr) {
    return;
  }
  ValidateJsonSchemaAnyOfContractTraversal(schema_root, any_of->AsArray(),
                                           schema_path, result);
}

}  // namespace objc3::io::json
