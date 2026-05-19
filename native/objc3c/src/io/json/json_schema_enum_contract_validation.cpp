#include "io/json/json_schema_enum_contract_validation.h"

#include "io/json/json_schema_enum_array_contract_validation.h"
#include "io/json/json_schema_enum_duplicates_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaEnumContract(const JsonValue &enum_values,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (!ValidateJsonSchemaEnumArrayContract(enum_values, schema_path, result)) {
    return;
  }
  ValidateJsonSchemaEnumDuplicateValuesContract(enum_values.AsArray(),
                                                schema_path, result);
}

}  // namespace objc3::io::json
