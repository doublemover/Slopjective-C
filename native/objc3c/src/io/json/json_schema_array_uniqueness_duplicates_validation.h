#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayUniqueItemDuplicates(
    const JsonValue::Array &array, const std::string &instance_path,
    const std::string &unique_items_schema_path, JsonSchemaResult &result);

}  // namespace objc3::io::json
