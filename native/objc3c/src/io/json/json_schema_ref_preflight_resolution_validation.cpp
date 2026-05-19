#include "io/json/json_schema_ref_preflight_resolution_validation.h"

#include "io/json/json_pointer.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

JsonSchemaRefPreflightResolutionValidation
ValidateJsonSchemaRefPreflightResolution(const JsonValue &schema_root,
                                         const JsonValue &ref_keyword,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  const JsonValue *resolved =
      ResolveLocalJsonPointerRef(schema_root, ref_keyword.AsString());
  if (resolved == nullptr) {
    AddJsonSchemaContractError(
        result, "unresolved_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
        "unresolved schema reference " + ref_keyword.AsString());
    JsonSchemaRefPreflightResolutionValidation invalid;
    invalid.valid = false;
    return invalid;
  }

  JsonSchemaRefPreflightResolutionValidation valid;
  valid.resolved_schema = resolved;
  return valid;
}

}  // namespace objc3::io::json
