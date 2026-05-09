#include "io/json/json_schema_node_preflight_validation.h"

#include "io/json/json_pointer.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaNodePreflight(const JsonValue &schema_root,
                                     const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_node", schema_path,
                               "schema node must be a JSON object");
    return false;
  }

  const JsonValue *ref = schema.Find("$ref");
  if (ref == nullptr) {
    return true;
  }
  if (!ref->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
        "$ref must be a local JSON pointer string");
    return false;
  }

  const JsonValue *resolved =
      ResolveLocalJsonPointerRef(schema_root, ref->AsString());
  if (resolved == nullptr) {
    AddJsonSchemaContractError(
        result, "unresolved_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
        "unresolved schema reference " + ref->AsString());
    return false;
  }

  ValidateJsonSchemaNode(schema_root, *resolved, payload, instance_path,
                         ref->AsString(), result);
  return false;
}

}  // namespace objc3::io::json
