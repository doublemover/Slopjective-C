#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaArrayItemsKeywordValidation {
  const JsonValue *value = nullptr;
  bool valid = true;
};

[[nodiscard]] JsonSchemaArrayItemsKeywordValidation
ValidateJsonSchemaArrayItemsKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
