#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaEnumContract(const JsonValue &enum_values,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result);

}  // namespace objc3::io::json
