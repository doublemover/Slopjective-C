#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaMaximumPayload(
    const JsonValue &maximum,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
