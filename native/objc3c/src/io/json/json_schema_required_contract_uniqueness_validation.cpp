#include "io/json/json_schema_required_contract_uniqueness_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredEntryUniqueness(
    const JsonValue::Array &required_entries, std::size_t entry_index,
    const std::string &entry_name, const std::string &schema_path,
    JsonSchemaResult &result) {
  for (std::size_t j = entry_index + 1; j < required_entries.size(); ++j) {
    if (required_entries[j].IsString() &&
        required_entries[j].AsString() == entry_name) {
      AddJsonSchemaContractError(
          result, "duplicate_required",
          JsonInstanceArrayElementPath(schema_path, j),
          "required property names must be unique");
    }
  }
}

}  // namespace objc3::io::json
