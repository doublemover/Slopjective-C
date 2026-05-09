#pragma once

#include <string>

#include "io/json/json_value.h"

namespace objc3::io::json {

std::string JsonSchemaValueTypeName(const JsonValue &value);
std::string DescribeExpectedJsonSchemaType(const JsonValue &schema_type);
bool JsonSchemaMatchesType(const JsonValue &schema_type,
                           const JsonValue &payload);

}  // namespace objc3::io::json
