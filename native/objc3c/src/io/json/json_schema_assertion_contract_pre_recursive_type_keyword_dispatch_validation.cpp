#include "io/json/json_schema_assertion_contract_pre_recursive_type_keyword_dispatch_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_type_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPreRecursiveTypeKeywordContractDispatch(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *type = schema.Find("type");
  if (type == nullptr) {
    return;
  }
  ValidateJsonSchemaTypeContract(
      *type, JsonSchemaKeywordPath(schema_path, "type"), result);
}

}  // namespace objc3::io::json
