#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaRefPreflightKeywordValidation {
  bool continue_current_schema = false;
  bool valid = true;
  const JsonValue *ref_keyword = nullptr;
};

[[nodiscard]] JsonSchemaRefPreflightKeywordValidation
ValidateJsonSchemaRefPreflightKeyword(const JsonValue &schema,
                                      const std::string &schema_path,
                                      JsonSchemaResult &result);

}  // namespace objc3::io::json
