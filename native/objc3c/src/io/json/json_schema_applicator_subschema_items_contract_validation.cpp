#include "io/json/json_schema_applicator_subschema_items_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaApplicatorItemsContract(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *items = schema.Find("items");
  if (items == nullptr) {
    return;
  }
  ValidateJsonSchemaNodeContract(
      schema_root, *items, JsonSchemaKeywordPath(schema_path, "items"),
      result);
}

}  // namespace objc3::io::json
