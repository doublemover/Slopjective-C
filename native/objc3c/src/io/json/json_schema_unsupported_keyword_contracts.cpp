#include "io/json/json_schema_unsupported_keyword_contracts.h"

#include <string_view>

#include "io/json/json_schema_errors.h"

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

void ValidateJsonSchemaUnsupportedKeywordContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  for (const auto &[key, value] : schema.AsObject()) {
    (void)value;
    if (!IsSupportedJsonSchemaKeyword(key)) {
      AddJsonSchemaContractError(
          result, "unsupported_keyword", JsonSchemaKeywordPath(schema_path, key),
          "unsupported JSON Schema keyword " + key);
    }
  }
}

}  // namespace objc3::io::json
