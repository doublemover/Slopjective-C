#include "io/json/json_schema_assertion_contract_post_recursive_enum_dispatch_validation.h"

#include "io/json/json_schema_enum_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaPostRecursiveEnumContractDispatch(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *enum_values = schema.Find("enum");
  if (enum_values == nullptr) {
    return;
  }
  ValidateJsonSchemaEnumContract(
      *enum_values, JsonSchemaKeywordPath(schema_path, "enum"), result);
}

}  // namespace objc3::io::json
