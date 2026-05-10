#include "io/json/json_schema_type_array_contract_validation.h"

#include <cstddef>
#include <string>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_type_name_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaTypeArrayContract(const JsonValue &schema_type,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result) {
  if (schema_type.AsArray().empty()) {
    AddJsonSchemaContractError(result, "invalid_type", schema_path,
                               "type array must not be empty");
    return;
  }
  for (std::size_t i = 0; i < schema_type.AsArray().size(); ++i) {
    const JsonValue &entry = schema_type.AsArray()[i];
    const std::string entry_path = JsonInstanceArrayElementPath(schema_path, i);
    if (!entry.IsString()) {
      AddJsonSchemaContractError(result, "invalid_type_entry", entry_path,
                                 "type array entries must be strings");
      continue;
    }
    if (!IsSupportedJsonSchemaTypeName(entry.AsString())) {
      AddJsonSchemaContractError(result, "unsupported_type", entry_path,
                                 "unsupported JSON Schema type " +
                                     entry.AsString());
    }
  }
}

}  // namespace objc3::io::json
