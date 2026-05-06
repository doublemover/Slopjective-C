#pragma once

#include <string>
#include <vector>

#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaResult {
  bool ok = true;
  std::vector<std::string> errors;
};

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema, const JsonValue &payload);

}  // namespace objc3::io::json
