#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaResolvedRefPreflight(
    const JsonValue &schema_root,
    const JsonValue &resolved_schema,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &resolved_schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
