#include "io/json/json_schema_ref_preflight_validation.h"

#include "io/json/json_schema_ref_preflight_delegation_validation.h"
#include "io/json/json_schema_ref_preflight_lookup_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaRefPreflight(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const JsonValue &payload,
                                    const std::string &instance_path,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  const JsonSchemaRefPreflightLookup lookup =
      ValidateJsonSchemaRefPreflightLookup(schema_root, schema, schema_path,
                                           result);
  if (lookup.continue_current_schema) {
    return true;
  }
  if (lookup.resolved_schema == nullptr || lookup.ref_keyword == nullptr) {
    return false;
  }
  ValidateJsonSchemaResolvedRefPreflight(
      schema_root, *lookup.resolved_schema, payload, instance_path,
      lookup.ref_keyword->AsString(), result);
  return false;
}

}  // namespace objc3::io::json
