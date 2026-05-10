#include "io/json/json_schema_type_array_contract_entry_validation.h"

#include <string>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_type_name_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaTypeArrayEntryContract(
    const JsonValue &entry,
    std::size_t entry_index,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const std::string entry_path =
      JsonInstanceArrayElementPath(schema_path, entry_index);
  if (!entry.IsString()) {
    AddJsonSchemaContractError(result, "invalid_type_entry", entry_path,
                               "type array entries must be strings");
    return;
  }
  if (!IsSupportedJsonSchemaTypeName(entry.AsString())) {
    AddJsonSchemaContractError(result, "unsupported_type", entry_path,
                               "unsupported JSON Schema type " +
                                   entry.AsString());
  }
}

}  // namespace objc3::io::json
