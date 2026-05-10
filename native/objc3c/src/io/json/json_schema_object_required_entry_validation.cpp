#include "io/json/json_schema_object_required_entry_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectRequiredEntries(
    const JsonValue &required, const JsonValue &payload,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue::Array &required_entries = required.AsArray();
  for (std::size_t i = 0; i < required_entries.size(); ++i) {
    const JsonValue &entry = required_entries[i];
    if (!entry.IsString()) {
      AddJsonSchemaContractError(
          result, "invalid_required_entry",
          JsonSchemaArrayElementPath(schema_path, "required", i),
          "required entries must be strings");
      continue;
    }
    if (payload.Find(entry.AsString()) == nullptr) {
      AddJsonSchemaPayloadError(
          result, "missing_required",
          JsonInstancePropertyPath(instance_path, entry.AsString()),
          JsonSchemaArrayElementPath(schema_path, "required", i),
          "missing required property " + entry.AsString());
    }
  }
}

}  // namespace objc3::io::json
