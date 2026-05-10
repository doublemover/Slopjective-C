#include "io/json/json_schema_required_contract_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_required_contract_entry_validation.h"
#include "io/json/json_schema_required_contract_uniqueness_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredContract(const JsonValue &required,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  if (!required.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_required", schema_path,
                               "required must be an array of strings");
    return;
  }
  const JsonValue::Array &required_entries = required.AsArray();
  for (std::size_t i = 0; i < required_entries.size(); ++i) {
    const JsonValue &entry = required_entries[i];
    if (!ValidateJsonSchemaRequiredEntryContract(entry, i, schema_path,
                                                 result)) {
      continue;
    }
    ValidateJsonSchemaRequiredEntryUniqueness(
        required_entries, i, entry.AsString(), schema_path, result);
  }
}

}  // namespace objc3::io::json
