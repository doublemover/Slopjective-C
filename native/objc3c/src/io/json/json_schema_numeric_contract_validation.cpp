#include "io/json/json_schema_numeric_contract_validation.h"

#include "io/json/json_schema_numeric_minimum_maximum_contract_validation.h"
#include "io/json/json_schema_numeric_nonnegative_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNumericAssertionContracts(const JsonValue &schema,
                                                 const std::string &schema_path,
                                                 JsonSchemaResult &result) {
  ValidateJsonSchemaMinimumMaximumContracts(schema, schema_path, result);
  ValidateJsonSchemaNonnegativeNumericKeywordContracts(schema, schema_path,
                                                       result);
}

}  // namespace objc3::io::json
