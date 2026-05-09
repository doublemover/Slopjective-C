#include "io/json/json_schema_applicator_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaPropertiesContract(const JsonValue &schema_root,
                                          const JsonValue &schema,
                                          const std::string &schema_path,
                                          JsonSchemaResult &result) {
  const JsonValue *properties = schema.Find("properties");
  if (properties == nullptr) {
    return;
  }
  const std::string properties_path =
      JsonSchemaKeywordPath(schema_path, "properties");
  if (!properties->IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_map", properties_path,
                               "schema map must be an object");
    return;
  }
  for (const auto &[key, child] : properties->AsObject()) {
    ValidateJsonSchemaNodeContract(
        schema_root, child, JsonInstancePropertyPath(properties_path, key),
        result);
  }
}

void ValidateJsonSchemaDefsContract(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  const JsonValue *defs = schema.Find("$defs");
  if (defs == nullptr) {
    return;
  }
  const std::string defs_path = JsonSchemaKeywordPath(schema_path, "$defs");
  if (!defs->IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_map", defs_path,
                               "schema map must be an object");
    return;
  }
  for (const auto &[key, child] : defs->AsObject()) {
    ValidateJsonSchemaNodeContract(
        schema_root, child, JsonInstancePropertyPath(defs_path, key), result);
  }
}

void ValidateJsonSchemaDefinitionsContract(const JsonValue &schema_root,
                                           const JsonValue &schema,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  const JsonValue *definitions = schema.Find("definitions");
  if (definitions == nullptr) {
    return;
  }
  const std::string definitions_path =
      JsonSchemaKeywordPath(schema_path, "definitions");
  if (!definitions->IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_map", definitions_path,
                               "schema map must be an object");
    return;
  }
  for (const auto &[key, child] : definitions->AsObject()) {
    ValidateJsonSchemaNodeContract(
        schema_root, child, JsonInstancePropertyPath(definitions_path, key),
        result);
  }
}

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
  ValidateJsonSchemaPropertiesContract(schema_root, schema, schema_path,
                                       result);
  ValidateJsonSchemaDefsContract(schema_root, schema, schema_path, result);
  ValidateJsonSchemaDefinitionsContract(schema_root, schema, schema_path,
                                        result);
  ValidateJsonSchemaAdditionalPropertiesContract(schema_root, schema,
                                                 schema_path, result);
  ValidateJsonSchemaItemsContract(schema_root, schema, schema_path, result);
  ValidateJsonSchemaContainsContract(schema_root, schema, schema_path, result);
}

}  // namespace objc3::io::json
