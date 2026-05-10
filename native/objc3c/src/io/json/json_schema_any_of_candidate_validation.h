#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaAnyOfCandidateMatch {
  bool matched = false;
  bool schema_failed = false;
};

JsonSchemaAnyOfCandidateMatch ValidateJsonSchemaAnyOfCandidates(
    const JsonValue &schema_root, const JsonValue::Array &candidates,
    const JsonValue &payload, const std::string &instance_path,
    const std::string &schema_path, JsonSchemaResult &result);

}  // namespace objc3::io::json
