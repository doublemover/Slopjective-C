#include "io/json/json_schema_assertion_contract_post_recursive_required_dispatch_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_required_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPostRecursiveRequiredContractDispatch(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *required = schema.Find("required");
  if (required == nullptr) {
    return;
  }
  ValidateJsonSchemaRequiredContract(
      *required, JsonSchemaKeywordPath(schema_path, "required"), result);
}

}  // namespace objc3::io::json
