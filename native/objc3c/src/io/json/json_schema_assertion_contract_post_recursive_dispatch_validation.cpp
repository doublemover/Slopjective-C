#include "io/json/json_schema_assertion_contract_post_recursive_dispatch_validation.h"

#include "io/json/json_schema_numeric_contract_validation.h"
#include "io/json/json_schema_pattern_contract_validation.h"
#include "io/json/json_schema_assertion_contract_post_recursive_enum_dispatch_validation.h"
#include "io/json/json_schema_assertion_contract_post_recursive_required_dispatch_validation.h"
#include "io/json/json_schema_unique_items_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPostRecursiveAssertionContractDispatch(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaPostRecursiveRequiredContractDispatch(schema, schema_path,
                                                          result);
  ValidateJsonSchemaPostRecursiveEnumContractDispatch(schema, schema_path,
                                                      result);
  ValidateJsonSchemaNumericAssertionContracts(schema, schema_path, result);
  ValidateJsonSchemaUniqueItemsContract(schema, schema_path, result);
  ValidateJsonSchemaPatternContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
