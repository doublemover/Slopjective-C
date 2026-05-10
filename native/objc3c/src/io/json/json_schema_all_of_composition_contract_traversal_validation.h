#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOfContractTraversal(
    const JsonValue &schema_root,
    const JsonValue::Array &all_of_schemas,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
