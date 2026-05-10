#include "io/json/json_schema_object_required_entry_validation.h"

#include <cstddef>

#include "io/json/json_schema_object_required_entry_type_validation.h"
#include "io/json/json_schema_object_required_missing_property_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectRequiredEntries(
    const JsonValue &required, const JsonValue &payload,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue::Array &required_entries = required.AsArray();
  for (std::size_t i = 0; i < required_entries.size(); ++i) {
    const JsonValue &entry = required_entries[i];
    if (!ValidateJsonSchemaObjectRequiredEntryType(entry, i, schema_path,
                                                   result)) {
      continue;
    }
    ValidateJsonSchemaObjectMissingRequiredProperty(
        payload, entry.AsString(), i, instance_path, schema_path, result);
  }
}

}  // namespace objc3::io::json
