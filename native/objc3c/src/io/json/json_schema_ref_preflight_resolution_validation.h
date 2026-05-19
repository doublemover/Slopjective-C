#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaRefPreflightResolutionValidation {
  bool valid = true;
  const JsonValue *resolved_schema = nullptr;
};

[[nodiscard]] JsonSchemaRefPreflightResolutionValidation
ValidateJsonSchemaRefPreflightResolution(const JsonValue &schema_root,
                                         const JsonValue &ref_keyword,
                                         const std::string &schema_path,
                                         JsonSchemaResult &result);

}  // namespace objc3::io::json
