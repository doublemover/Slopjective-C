#include "io/json/json_schema_type_contract_validation.h"

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_type_array_contract_validation.h"
#include "io/json/json_schema_type_name_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaTypeContract(const JsonValue &schema_type,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (schema_type.IsString()) {
    if (!IsSupportedJsonSchemaTypeName(schema_type.AsString())) {
      AddJsonSchemaContractError(result, "unsupported_type", schema_path,
                                 "unsupported JSON Schema type " +
                                     schema_type.AsString());
    }
    return;
  }
  if (!schema_type.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_type", schema_path,
                               "type must be a string or an array of strings");
    return;
  }
  ValidateJsonSchemaTypeArrayContract(schema_type, schema_path, result);
}

}  // namespace objc3::io::json
