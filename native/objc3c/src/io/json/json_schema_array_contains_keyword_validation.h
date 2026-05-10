#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaArrayContainsKeywordValidation {
  const JsonValue *value = nullptr;
  bool valid = true;
};

[[nodiscard]] JsonSchemaArrayContainsKeywordValidation
ValidateJsonSchemaArrayContainsKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
