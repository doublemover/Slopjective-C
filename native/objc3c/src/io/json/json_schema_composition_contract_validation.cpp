#include "io/json/json_schema_composition_contract_validation.h"

#include "io/json/json_schema_all_of_composition_contract_validation.h"
#include "io/json/json_schema_any_of_composition_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaCompositionContracts(const JsonValue &schema_root,
                                            const JsonValue &schema,
                                            const std::string &schema_path,
                                            JsonSchemaResult &result) {
  ValidateJsonSchemaAllOfContract(schema_root, schema, schema_path, result);
  ValidateJsonSchemaAnyOfContract(schema_root, schema, schema_path, result);
}

}  // namespace objc3::io::json
