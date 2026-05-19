#include "io/json/json_schema_all_of_composition_contract_validation.h"

#include "io/json/json_schema_all_of_composition_contract_array_validation.h"
#include "io/json/json_schema_all_of_composition_contract_traversal_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOfContract(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *all_of =
      ValidateJsonSchemaAllOfContractArray(schema, schema_path, result);
  if (all_of == nullptr) {
    return;
  }
  ValidateJsonSchemaAllOfContractTraversal(schema_root, all_of->AsArray(),
                                           schema_path, result);
}

}  // namespace objc3::io::json
