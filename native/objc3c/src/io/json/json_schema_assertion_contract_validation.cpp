#include "io/json/json_schema_assertion_contract_validation.h"

#include "io/json/json_schema_assertion_contract_post_recursive_dispatch_validation.h"
#include "io/json/json_schema_assertion_contract_pre_recursive_type_dispatch_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPreRecursiveAssertionContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaPreRecursiveTypeContractDispatch(schema, schema_path,
                                                     result);
}

void ValidateJsonSchemaPostRecursiveAssertionContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaPostRecursiveAssertionContractDispatch(schema, schema_path,
                                                           result);
}

}  // namespace objc3::io::json
