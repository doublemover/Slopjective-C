#include "io/json/json_schema_assertion_contract_post_recursive_dispatch_validation.h"

#include "io/json/json_schema_enum_contract_validation.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_numeric_contract_validation.h"
#include "io/json/json_schema_pattern_contract_validation.h"
#include "io/json/json_schema_required_contract_validation.h"
#include "io/json/json_schema_unique_items_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaPostRecursiveAssertionContractDispatch(
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (const JsonValue *required = schema.Find("required");
      required != nullptr) {
    ValidateJsonSchemaRequiredContract(
        *required, JsonSchemaKeywordPath(schema_path, "required"), result);
  }
  if (const JsonValue *enum_values = schema.Find("enum");
      enum_values != nullptr) {
    ValidateJsonSchemaEnumContract(
        *enum_values, JsonSchemaKeywordPath(schema_path, "enum"), result);
  }
  ValidateJsonSchemaNumericAssertionContracts(schema, schema_path, result);
  ValidateJsonSchemaUniqueItemsContract(schema, schema_path, result);
  ValidateJsonSchemaPatternContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
