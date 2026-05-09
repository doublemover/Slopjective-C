#include "io/json/json_schema_applicator_contract_validation.h"

#include "io/json/json_schema_applicator_map_contract_validation.h"
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

void ValidateJsonSchemaApplicatorContracts(const JsonValue &schema_root,
                                           const JsonValue &schema,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  ValidateJsonSchemaApplicatorMapContracts(schema_root, schema, schema_path,
                                           result);
  ValidateJsonSchemaAdditionalPropertiesContract(schema_root, schema,
                                                 schema_path, result);
  ValidateJsonSchemaItemsContract(schema_root, schema, schema_path, result);
  ValidateJsonSchemaContainsContract(schema_root, schema, schema_path, result);
}

}  // namespace objc3::io::json
