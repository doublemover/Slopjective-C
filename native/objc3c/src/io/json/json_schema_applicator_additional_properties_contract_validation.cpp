#include "io/json/json_schema_applicator_additional_properties_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaAdditionalPropertiesContract(
    const JsonValue &schema_root, const JsonValue &schema,
    const std::string &schema_path, JsonSchemaResult &result) {
  const JsonValue *additional = schema.Find("additionalProperties");
  if (additional == nullptr) {
    return;
  }
  if (additional->IsObject()) {
    ValidateJsonSchemaNodeContract(
        schema_root, *additional,
        JsonSchemaKeywordPath(schema_path, "additionalProperties"), result);
  } else if (!additional->IsBool()) {
    AddJsonSchemaContractError(
        result, "invalid_additional_properties",
        JsonSchemaKeywordPath(schema_path, "additionalProperties"),
        "additionalProperties must be false, true, or a schema object");
  }
}

}  // namespace objc3::io::json
