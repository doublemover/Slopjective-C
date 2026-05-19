#include "io/json/json_schema_type_array_contract_validation.h"

#include <cstddef>

#include "io/json/json_schema_type_array_contract_empty_validation.h"
#include "io/json/json_schema_type_array_contract_entry_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaTypeArrayContract(const JsonValue &schema_type,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  if (!ValidateJsonSchemaTypeArrayNotEmptyContract(schema_type, schema_path,
                                                   result)) {
    return;
  }
  const JsonValue::Array &type_names = schema_type.AsArray();
  for (std::size_t i = 0; i < type_names.size(); ++i) {
    ValidateJsonSchemaTypeArrayEntryContract(type_names[i], i, schema_path,
                                             result);
  }
}

}  // namespace objc3::io::json
