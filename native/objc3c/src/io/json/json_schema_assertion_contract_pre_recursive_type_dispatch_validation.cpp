#include "io/json/json_schema_assertion_contract_pre_recursive_type_dispatch_validation.h"

#include "io/json/json_schema_assertion_contract_pre_recursive_type_keyword_dispatch_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPreRecursiveTypeContractDispatch(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaPreRecursiveTypeKeywordContractDispatch(schema,
                                                            schema_path,
                                                            result);
}

}  // namespace objc3::io::json
