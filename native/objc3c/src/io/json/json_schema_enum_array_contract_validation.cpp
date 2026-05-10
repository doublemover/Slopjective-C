#include "io/json/json_schema_enum_array_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaEnumArrayContract(const JsonValue &enum_values,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  if (!enum_values.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_enum", schema_path,
                               "enum must be an array");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
