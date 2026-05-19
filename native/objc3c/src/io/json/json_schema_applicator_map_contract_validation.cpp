#include "io/json/json_schema_applicator_map_contract_validation.h"

#include "io/json/json_schema_applicator_definition_map_contract_validation.h"
#include "io/json/json_schema_applicator_properties_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaApplicatorMapContracts(const JsonValue &schema_root,
                                              const JsonValue &schema,
                                              const std::string &schema_path,
                                              JsonSchemaResult &result) {
  ValidateJsonSchemaPropertiesContract(schema_root, schema, schema_path,
                                       result);
  ValidateJsonSchemaDefinitionMapContracts(schema_root, schema, schema_path,
                                           result);
}

}  // namespace objc3::io::json
