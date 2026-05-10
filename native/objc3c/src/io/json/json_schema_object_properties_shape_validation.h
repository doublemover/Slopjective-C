#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

[[nodiscard]] const JsonValue *ValidateJsonSchemaObjectPropertiesShape(
    const JsonValue *properties,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
