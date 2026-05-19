#include "io/json/json_schema_required_contract_array_shape_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaRequiredArrayShape(const JsonValue &required,
                                          const std::string &schema_path,
                                          JsonSchemaResult &result) {
  if (!required.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_required", schema_path,
                               "required must be an array of strings");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
