#pragma once

#include <cstddef>
#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

[[nodiscard]] JsonSchemaResult ValidateJsonSchemaAnyOfCandidateProbe(
    const JsonValue &schema_root,
    const JsonValue &candidate,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    std::size_t candidate_index);

}  // namespace objc3::io::json
