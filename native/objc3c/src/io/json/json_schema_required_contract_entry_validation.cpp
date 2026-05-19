#include "io/json/json_schema_required_contract_entry_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaRequiredEntryContract(
    const JsonValue &entry, std::size_t entry_index,
    const std::string &schema_path, JsonSchemaResult &result) {
  const std::string entry_path =
      JsonInstanceArrayElementPath(schema_path, entry_index);
  if (!entry.IsString()) {
    AddJsonSchemaContractError(result, "invalid_required_entry", entry_path,
                               "required entries must be strings");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
