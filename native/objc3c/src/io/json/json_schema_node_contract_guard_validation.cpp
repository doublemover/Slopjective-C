#include "io/json/json_schema_node_contract_guard_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaNodeContractGuard(const JsonValue &schema,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_node", schema_path,
                               "schema node must be a JSON object");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
