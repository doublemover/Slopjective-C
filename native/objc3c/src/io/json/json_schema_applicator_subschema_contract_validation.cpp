#include "io/json/json_schema_applicator_subschema_contract_validation.h"

#include "io/json/json_schema_applicator_subschema_contains_contract_validation.h"
#include "io/json/json_schema_applicator_subschema_items_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaApplicatorSubschemaContracts(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaApplicatorItemsContract(schema_root, schema, schema_path,
                                            result);
  ValidateJsonSchemaApplicatorContainsContract(schema_root, schema,
                                               schema_path, result);
}

}  // namespace objc3::io::json
