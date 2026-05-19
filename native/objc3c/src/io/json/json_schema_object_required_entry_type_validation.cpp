#include "io/json/json_schema_object_required_entry_type_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

bool ValidateJsonSchemaObjectRequiredEntryType(
    const JsonValue &entry,
    std::size_t entry_index,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!entry.IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_required_entry",
        JsonSchemaArrayElementPath(schema_path, "required", entry_index),
        "required entries must be strings");
    return false;
  }
  return true;
}

}  // namespace objc3::io::json
