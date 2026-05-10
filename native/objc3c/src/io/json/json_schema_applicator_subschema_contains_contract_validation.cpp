#include "io/json/json_schema_applicator_subschema_contains_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaApplicatorContainsContract(
    const JsonValue &schema_root,
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

}  // namespace objc3::io::json
