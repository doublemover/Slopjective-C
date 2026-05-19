#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

bool ValidateJsonSchemaUnexpectedAdditionalProperty(
    const JsonValue &additional_properties, const std::string &key,
    const std::string &instance_path, const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
