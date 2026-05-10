#include "io/json/json_schema_assertion_contract_pre_recursive_type_dispatch_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_type_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPreRecursiveTypeContractDispatch(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (const JsonValue *type = schema.Find("type"); type != nullptr) {
    ValidateJsonSchemaTypeContract(*type,
                                   JsonSchemaKeywordPath(schema_path, "type"),
                                   result);
  }
}

}  // namespace objc3::io::json
