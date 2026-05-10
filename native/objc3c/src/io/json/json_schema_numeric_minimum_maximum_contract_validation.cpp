#include "io/json/json_schema_numeric_minimum_maximum_contract_validation.h"

#include "io/json/json_schema_numeric_maximum_contract_validation.h"
#include "io/json/json_schema_numeric_minimum_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaMinimumMaximumContracts(const JsonValue &schema,
                                               const std::string &schema_path,
                                               JsonSchemaResult &result) {
  ValidateJsonSchemaMinimumContract(schema, schema_path, result);
  ValidateJsonSchemaMaximumContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
