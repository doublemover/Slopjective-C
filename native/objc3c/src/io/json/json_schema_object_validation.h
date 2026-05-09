#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaObjectFields(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const JsonValue &payload,
                                    const JsonValue *properties,
                                    const std::string &path,
                                    JsonSchemaResult &result);

}  // namespace objc3::io::json

