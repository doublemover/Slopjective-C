#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_schema_any_of_candidate_validation.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

[[nodiscard]] JsonSchemaAnyOfCandidateMatch ValidateJsonSchemaAnyOfMatch(
    const JsonValue &schema_root,
    const JsonValue::Array &candidates,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
