#pragma once

#include <string>

#include "io/json/json_schema.h"

namespace objc3::io::json {

void AddJsonSchemaError(JsonSchemaResult &result, std::string message);

}  // namespace objc3::io::json

