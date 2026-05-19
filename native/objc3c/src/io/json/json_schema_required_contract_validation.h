#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredContract(const JsonValue &required,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result);

}  // namespace objc3::io::json
