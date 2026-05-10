#pragma once

#include "io/json/json_schema.h"

namespace objc3::io::json {

[[nodiscard]] bool PropagateJsonSchemaAnyOfCandidateSchemaIssues(
    JsonSchemaResult &probe,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
