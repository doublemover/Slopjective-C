#include "io/json/json_schema_ref_preflight_lookup_validation.h"

#include "io/json/json_schema_ref_preflight_keyword_validation.h"
#include "io/json/json_schema_ref_preflight_resolution_validation.h"

namespace objc3::io::json {

JsonSchemaRefPreflightLookup ValidateJsonSchemaRefPreflightLookup(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonSchemaRefPreflightKeywordValidation keyword =
      ValidateJsonSchemaRefPreflightKeyword(schema, schema_path, result);
  if (keyword.continue_current_schema) {
    return JsonSchemaRefPreflightLookup{true, nullptr, nullptr};
  }
  if (!keyword.valid || keyword.ref_keyword == nullptr) {
    return JsonSchemaRefPreflightLookup{false, keyword.ref_keyword, nullptr};
  }

  const JsonSchemaRefPreflightResolutionValidation resolution =
      ValidateJsonSchemaRefPreflightResolution(schema_root, *keyword.ref_keyword,
                                              schema_path, result);
  if (!resolution.valid || resolution.resolved_schema == nullptr) {
    return JsonSchemaRefPreflightLookup{false, keyword.ref_keyword, nullptr};
  }

  return JsonSchemaRefPreflightLookup{false, keyword.ref_keyword,
                                      resolution.resolved_schema};
}

}  // namespace objc3::io::json
