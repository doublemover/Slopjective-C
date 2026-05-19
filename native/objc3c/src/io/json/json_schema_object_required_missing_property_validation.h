#pragma once

#include <cstddef>
#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectMissingRequiredProperty(
    const JsonValue &payload,
    const std::string &entry_name,
    std::size_t entry_index,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
