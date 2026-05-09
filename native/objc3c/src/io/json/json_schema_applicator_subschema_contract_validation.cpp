#include "io/json/json_schema_applicator_subschema_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

void ValidateJsonSchemaItemsContract(const JsonValue &schema_root,
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

void ValidateJsonSchemaContainsContract(const JsonValue &schema_root,
                                        const JsonValue &schema,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  const JsonValue *contains = schema.Find("contains");
  if (contains == nullptr) {
    return;
  }
  ValidateJsonSchemaNodeContract(
      schema_root, *contains, JsonSchemaKeywordPath(schema_path, "contains"),
      result);
}

}  // namespace

void ValidateJsonSchemaApplicatorSubschemaContracts(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaItemsContract(schema_root, schema, schema_path, result);
  ValidateJsonSchemaContainsContract(schema_root, schema, schema_path, result);
}

}  // namespace objc3::io::json
