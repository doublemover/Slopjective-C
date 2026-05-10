#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_schema_any_of_candidate_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAnyOfMismatch(
    const JsonSchemaAnyOfCandidateMatch &match,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
