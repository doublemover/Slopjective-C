#include "io/json/json_schema.h"

#include <string_view>

#include "io/json/json_pointer.h"
#include "io/json/json_schema_annotation_contract_validation.h"
#include "io/json/json_schema_applicator_contract_validation.h"
#include "io/json/json_schema_composition_contract_validation.h"
#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_enum_contract_validation.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_numeric_contract_validation.h"
#include "io/json/json_schema_pattern_contract_validation.h"
#include "io/json/json_schema_required_contract_validation.h"
#include "io/json/json_schema_type_contract_validation.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {
namespace {

bool IsJsonSchemaAnnotationKeyword(std::string_view key) {
  return key == "$schema" || key == "$id" || key == "$comment" ||
         key == "title" || key == "description" || key == "format";
}

bool IsJsonSchemaApplicatorKeyword(std::string_view key) {
  return key == "$ref" || key == "$defs" || key == "definitions" ||
         key == "allOf" || key == "anyOf" || key == "properties" ||
         key == "additionalProperties" || key == "items" ||
         key == "contains";
}

bool IsJsonSchemaAssertionKeyword(std::string_view key) {
  return key == "type" || key == "required" || key == "const" ||
         key == "enum" || key == "minimum" || key == "maximum" ||
         key == "minLength" || key == "maxLength" || key == "pattern" ||
         key == "minItems" || key == "maxItems" || key == "uniqueItems";
}

bool IsSupportedJsonSchemaKeyword(std::string_view key) {
  return IsJsonSchemaAnnotationKeyword(key) ||
         IsJsonSchemaApplicatorKeyword(key) ||
         IsJsonSchemaAssertionKeyword(key);
}

}  // namespace

void ValidateJsonSchemaNodeContract(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_node", schema_path,
                               "schema node must be a JSON object");
    return;
  }
  for (const auto &[key, value] : schema.AsObject()) {
    (void)value;
    if (!IsSupportedJsonSchemaKeyword(key)) {
      AddJsonSchemaContractError(
          result, "unsupported_keyword",
          JsonSchemaKeywordPath(schema_path, key),
          "unsupported JSON Schema keyword " + key);
    }
  }
  ValidateJsonSchemaAnnotationContracts(schema, schema_path, result);

  if (const JsonValue *ref = schema.Find("$ref"); ref != nullptr) {
    if (!ref->IsString()) {
      AddJsonSchemaContractError(result, "invalid_ref",
                                 JsonSchemaKeywordPath(schema_path, "$ref"),
                                 "$ref must be a local JSON pointer string");
    } else if (ResolveLocalJsonPointerRef(schema_root, ref->AsString()) ==
               nullptr) {
      AddJsonSchemaContractError(
          result, "unresolved_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
          "unresolved schema reference " + ref->AsString());
    }
  }
  if (const JsonValue *type = schema.Find("type"); type != nullptr) {
    ValidateJsonSchemaTypeContract(*type,
                                   JsonSchemaKeywordPath(schema_path, "type"),
                                   result);
  }
  ValidateJsonSchemaCompositionContracts(schema_root, schema, schema_path,
                                         result);
  ValidateJsonSchemaApplicatorContracts(schema_root, schema, schema_path,
                                        result);
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
  if (const JsonValue *unique = schema.Find("uniqueItems");
      unique != nullptr && !unique->IsBool()) {
    AddJsonSchemaContractError(
        result, "invalid_unique_items",
        JsonSchemaKeywordPath(schema_path, "uniqueItems"),
        "uniqueItems must be a boolean");
  }
  ValidateJsonSchemaPatternContract(schema, schema_path, result);
}

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema,
                                    const JsonValue &payload) {
  JsonSchemaResult result;
  ValidateJsonSchemaNodeContract(schema, schema, "$", result);
  if (!result.ok) {
    return result;
  }
  ValidateJsonSchemaNode(schema, schema, payload, "$", "$", result);
  return result;
}

}  // namespace objc3::io::json
