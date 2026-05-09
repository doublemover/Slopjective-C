#pragma once

#include <string>

#include "io/json/json_schema.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

void ValidateJsonSchemaPreRecursiveAssertionContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result);

void ValidateJsonSchemaPostRecursiveAssertionContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result);

}  // namespace objc3::io::json
