#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaAnyOfContractTraversal(
    const JsonValue &schema_root,
    const JsonValue::Array &any_of_schemas,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
