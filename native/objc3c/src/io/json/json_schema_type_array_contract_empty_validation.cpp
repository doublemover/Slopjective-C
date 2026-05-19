#include "io/json/json_schema_type_array_contract_empty_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaTypeArrayNotEmptyContract(
    const JsonValue &schema_type,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (schema_type.AsArray().empty()) {
    AddJsonSchemaContractError(result, "invalid_type", schema_path,
                               "type array must not be empty");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
