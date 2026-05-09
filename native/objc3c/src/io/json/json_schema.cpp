#include "io/json/json_schema.h"

#include <cstddef>
#include <regex>
#include <string_view>

#include "io/json/json_equivalence.h"
#include "io/json/json_pointer.h"
#include "io/json/json_schema_composition_contract_validation.h"
#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"
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

bool IsSupportedJsonSchemaTypeName(std::string_view type) {
  return type == "null" || type == "boolean" || type == "number" ||
         type == "integer" || type == "string" || type == "array" ||
         type == "object";
}

void ValidateJsonSchemaTypeContract(const JsonValue &schema_type,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (schema_type.IsString()) {
    if (!IsSupportedJsonSchemaTypeName(schema_type.AsString())) {
      AddJsonSchemaContractError(result, "unsupported_type", schema_path,
                                 "unsupported JSON Schema type " +
                                     schema_type.AsString());
    }
    return;
  }
  if (!schema_type.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_type", schema_path,
                               "type must be a string or an array of strings");
    return;
  }
  if (schema_type.AsArray().empty()) {
    AddJsonSchemaContractError(result, "invalid_type", schema_path,
                               "type array must not be empty");
    return;
  }
  for (std::size_t i = 0; i < schema_type.AsArray().size(); ++i) {
    const JsonValue &entry = schema_type.AsArray()[i];
    const std::string entry_path = JsonInstanceArrayElementPath(schema_path, i);
    if (!entry.IsString()) {
      AddJsonSchemaContractError(result, "invalid_type_entry", entry_path,
                                 "type array entries must be strings");
      continue;
    }
    if (!IsSupportedJsonSchemaTypeName(entry.AsString())) {
      AddJsonSchemaContractError(result, "unsupported_type", entry_path,
                                 "unsupported JSON Schema type " +
                                     entry.AsString());
    }
  }
}

void ValidateJsonSchemaStringAnnotation(const JsonValue &schema,
                                        std::string_view keyword,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  const JsonValue *value = schema.Find(keyword);
  if (value != nullptr && !value->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_annotation",
        JsonSchemaKeywordPath(schema_path, keyword),
        std::string(keyword) + " must be a string when present");
  }
}

void ValidateJsonSchemaNumberKeyword(const JsonValue &schema,
                                     std::string_view keyword,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *value = schema.Find(keyword);
  if (value != nullptr && !value->IsNumber()) {
    AddJsonSchemaContractError(
        result, "invalid_numeric_keyword",
        JsonSchemaKeywordPath(schema_path, keyword),
        std::string(keyword) + " must be a number");
  }
}

void ValidateJsonSchemaNonnegativeNumberKeyword(const JsonValue &schema,
                                                std::string_view keyword,
                                                const std::string &schema_path,
                                                JsonSchemaResult &result) {
  const JsonValue *value = schema.Find(keyword);
  if (value == nullptr) {
    return;
  }
  ValidateJsonSchemaNumberKeyword(schema, keyword, schema_path, result);
  if (value->IsNumber() && value->AsNumber() < 0.0) {
    AddJsonSchemaContractError(
        result, "invalid_nonnegative_keyword",
        JsonSchemaKeywordPath(schema_path, keyword),
        std::string(keyword) + " must be zero or greater");
  }
}

void ValidateJsonSchemaObjectOfSchemas(const JsonValue &schema_root,
                                       const JsonValue &container,
                                       const std::string &schema_path,
                                       JsonSchemaResult &result) {
  if (!container.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_map", schema_path,
                               "schema map must be an object");
    return;
  }
  for (const auto &[key, child] : container.AsObject()) {
    ValidateJsonSchemaNodeContract(schema_root, child,
                                   JsonInstancePropertyPath(schema_path, key),
                                   result);
  }
}

void ValidateJsonSchemaEnumContract(const JsonValue &enum_values,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (!enum_values.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_enum", schema_path,
                               "enum must be an array");
    return;
  }
  for (std::size_t i = 0; i < enum_values.AsArray().size(); ++i) {
    for (std::size_t j = i + 1; j < enum_values.AsArray().size(); ++j) {
      if (JsonEquals(enum_values.AsArray()[i], enum_values.AsArray()[j])) {
        AddJsonSchemaContractError(
            result, "duplicate_enum",
            JsonInstanceArrayElementPath(schema_path, j),
            "enum values must be unique");
      }
    }
  }
}

void ValidateJsonSchemaRequiredContract(const JsonValue &required,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  if (!required.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_required", schema_path,
                               "required must be an array of strings");
    return;
  }
  for (std::size_t i = 0; i < required.AsArray().size(); ++i) {
    const JsonValue &entry = required.AsArray()[i];
    const std::string entry_path = JsonInstanceArrayElementPath(schema_path, i);
    if (!entry.IsString()) {
      AddJsonSchemaContractError(result, "invalid_required_entry", entry_path,
                                 "required entries must be strings");
      continue;
    }
    for (std::size_t j = i + 1; j < required.AsArray().size(); ++j) {
      if (required.AsArray()[j].IsString() &&
          required.AsArray()[j].AsString() == entry.AsString()) {
        AddJsonSchemaContractError(
            result, "duplicate_required",
            JsonInstanceArrayElementPath(schema_path, j),
            "required property names must be unique");
      }
    }
  }
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
  ValidateJsonSchemaStringAnnotation(schema, "$schema", schema_path, result);
  ValidateJsonSchemaStringAnnotation(schema, "$id", schema_path, result);
  ValidateJsonSchemaStringAnnotation(schema, "$comment", schema_path, result);
  ValidateJsonSchemaStringAnnotation(schema, "title", schema_path, result);
  ValidateJsonSchemaStringAnnotation(schema, "description", schema_path,
                                     result);
  ValidateJsonSchemaStringAnnotation(schema, "format", schema_path, result);

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
  if (const JsonValue *properties = schema.Find("properties");
      properties != nullptr) {
    ValidateJsonSchemaObjectOfSchemas(
        schema_root, *properties,
        JsonSchemaKeywordPath(schema_path, "properties"), result);
  }
  if (const JsonValue *defs = schema.Find("$defs"); defs != nullptr) {
    ValidateJsonSchemaObjectOfSchemas(
        schema_root, *defs, JsonSchemaKeywordPath(schema_path, "$defs"),
        result);
  }
  if (const JsonValue *defs = schema.Find("definitions"); defs != nullptr) {
    ValidateJsonSchemaObjectOfSchemas(
        schema_root, *defs, JsonSchemaKeywordPath(schema_path, "definitions"),
        result);
  }
  if (const JsonValue *additional = schema.Find("additionalProperties");
      additional != nullptr) {
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
  if (const JsonValue *items = schema.Find("items"); items != nullptr) {
    ValidateJsonSchemaNodeContract(
        schema_root, *items, JsonSchemaKeywordPath(schema_path, "items"),
        result);
  }
  if (const JsonValue *contains = schema.Find("contains");
      contains != nullptr) {
    ValidateJsonSchemaNodeContract(
        schema_root, *contains, JsonSchemaKeywordPath(schema_path, "contains"),
        result);
  }
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
  ValidateJsonSchemaNumberKeyword(schema, "minimum", schema_path, result);
  ValidateJsonSchemaNumberKeyword(schema, "maximum", schema_path, result);
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "minLength", schema_path,
                                             result);
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "maxLength", schema_path,
                                             result);
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "minItems", schema_path,
                                             result);
  ValidateJsonSchemaNonnegativeNumberKeyword(schema, "maxItems", schema_path,
                                             result);
  if (const JsonValue *unique = schema.Find("uniqueItems");
      unique != nullptr && !unique->IsBool()) {
    AddJsonSchemaContractError(
        result, "invalid_unique_items",
        JsonSchemaKeywordPath(schema_path, "uniqueItems"),
        "uniqueItems must be a boolean");
  }
  if (const JsonValue *pattern = schema.Find("pattern"); pattern != nullptr) {
    if (!pattern->IsString()) {
      AddJsonSchemaContractError(
          result, "invalid_pattern",
          JsonSchemaKeywordPath(schema_path, "pattern"),
          "pattern must be a string");
    } else {
      try {
        (void)std::regex(pattern->AsString());
      } catch (const std::regex_error &) {
        AddJsonSchemaContractError(
            result, "invalid_pattern",
            JsonSchemaKeywordPath(schema_path, "pattern"),
            "pattern is not a valid regular expression");
      }
    }
  }
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
