#include "io/json/json_schema_ref_preflight_delegation_validation.h"

#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaResolvedRefPreflight(
    const JsonValue &schema_root,
    const JsonValue &resolved_schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &resolved_schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaNode(schema_root, resolved_schema, payload, instance_path,
                         resolved_schema_path, result);
}

}  // namespace objc3::io::json
