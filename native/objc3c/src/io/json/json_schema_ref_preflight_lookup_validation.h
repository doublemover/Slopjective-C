#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaRefPreflightLookup {
  bool continue_current_schema = false;
  const JsonValue *ref_keyword = nullptr;
  const JsonValue *resolved_schema = nullptr;
};

[[nodiscard]] JsonSchemaRefPreflightLookup ValidateJsonSchemaRefPreflightLookup(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
