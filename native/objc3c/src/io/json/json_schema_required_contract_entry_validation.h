#pragma once

#include <cstddef>
#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

[[nodiscard]] bool ValidateJsonSchemaRequiredEntryContract(
    const JsonValue &entry, std::size_t entry_index,
    const std::string &schema_path, JsonSchemaResult &result);

}  // namespace objc3::io::json
