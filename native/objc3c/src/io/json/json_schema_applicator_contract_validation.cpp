#include "io/json/json_schema_applicator_contract_validation.h"

#include "io/json/json_schema_applicator_additional_properties_contract_validation.h"
#include "io/json/json_schema_applicator_map_contract_validation.h"
#include "io/json/json_schema_applicator_subschema_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaApplicatorContracts(const JsonValue &schema_root,
                                           const JsonValue &schema,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  ValidateJsonSchemaApplicatorMapContracts(schema_root, schema, schema_path,
                                           result);
  ValidateJsonSchemaAdditionalPropertiesContract(schema_root, schema,
                                                 schema_path, result);
  ValidateJsonSchemaApplicatorSubschemaContracts(schema_root, schema,
                                                 schema_path, result);
}

}  // namespace objc3::io::json
