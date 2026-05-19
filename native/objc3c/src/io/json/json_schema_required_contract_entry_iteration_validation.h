#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredEntryIteration(
    const JsonValue::Array &required_entries,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
