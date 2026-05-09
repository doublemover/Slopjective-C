#include "io/json/json_schema_required_contract_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredContract(const JsonValue &required,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  if (!required.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_required", schema_path,
                               "required must be an array of strings");
    return;
  }
  for (std::size_t i = 0; i < required.AsArray().size(); ++i) {
    const JsonValue &entry = required.AsArray()[i];
    const std::string entry_path = JsonInstanceArrayElementPath(schema_path, i);
    if (!entry.IsString()) {
      AddJsonSchemaContractError(result, "invalid_required_entry", entry_path,
                                 "required entries must be strings");
      continue;
    }
    for (std::size_t j = i + 1; j < required.AsArray().size(); ++j) {
      if (required.AsArray()[j].IsString() &&
          required.AsArray()[j].AsString() == entry.AsString()) {
        AddJsonSchemaContractError(
            result, "duplicate_required",
            JsonInstanceArrayElementPath(schema_path, j),
            "required property names must be unique");
      }
    }
  }
}

}  // namespace objc3::io::json
