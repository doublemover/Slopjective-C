#include "io/json/json_schema_applicator_definition_map_contract_validation.h"

#include "io/json/json_schema_applicator_definitions_contract_validation.h"
#include "io/json/json_schema_applicator_defs_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaDefinitionMapContracts(const JsonValue &schema_root,
                                              const JsonValue &schema,
                                              const std::string &schema_path,
                                              JsonSchemaResult &result) {
  ValidateJsonSchemaDefsContract(schema_root, schema, schema_path, result);
  ValidateJsonSchemaDefinitionsContract(schema_root, schema, schema_path,
                                        result);
}

}  // namespace objc3::io::json
