#include "io/json/json_schema_ref_preflight_lookup_validation.h"

#include "io/json/json_pointer.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaRefPreflightLookup ValidateJsonSchemaRefPreflightLookup(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *ref = schema.Find("$ref");
  if (ref == nullptr) {
    return JsonSchemaRefPreflightLookup{true, nullptr, nullptr};
  }
  if (!ref->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
        "$ref must be a local JSON pointer string");
    return JsonSchemaRefPreflightLookup{false, ref, nullptr};
  }

  const JsonValue *resolved =
      ResolveLocalJsonPointerRef(schema_root, ref->AsString());
  if (resolved == nullptr) {
    AddJsonSchemaContractError(
        result, "unresolved_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
        "unresolved schema reference " + ref->AsString());
    return JsonSchemaRefPreflightLookup{false, ref, nullptr};
  }

  return JsonSchemaRefPreflightLookup{false, ref, resolved};
}

}  // namespace objc3::io::json
