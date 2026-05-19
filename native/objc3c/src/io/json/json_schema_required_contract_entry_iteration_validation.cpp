#include "io/json/json_schema_required_contract_entry_iteration_validation.h"

#include <cstddef>

#include "io/json/json_schema_required_contract_entry_validation.h"
#include "io/json/json_schema_required_contract_uniqueness_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredEntryIteration(
    const JsonValue::Array &required_entries,
    const std::string &schema_path,
    JsonSchemaResult &result) {
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
