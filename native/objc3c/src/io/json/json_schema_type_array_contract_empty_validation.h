#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

[[nodiscard]] bool ValidateJsonSchemaTypeArrayNotEmptyContract(
    const JsonValue &schema_type,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
