#pragma once

#include <optional>
#include <regex>
#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaStringPatternKeywordValidation {
  std::optional<std::regex> regex;
};

[[nodiscard]] JsonSchemaStringPatternKeywordValidation
ValidateJsonSchemaStringPatternKeyword(
    const JsonValue &schema,
    const JsonValue &payload,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
