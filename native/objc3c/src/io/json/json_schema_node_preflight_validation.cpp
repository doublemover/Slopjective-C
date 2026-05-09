#include "io/json/json_schema_node_preflight_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_ref_preflight_validation.h"

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

  return ValidateJsonSchemaRefPreflight(schema_root, schema, payload,
                                        instance_path, schema_path, result);
}

}  // namespace objc3::io::json
