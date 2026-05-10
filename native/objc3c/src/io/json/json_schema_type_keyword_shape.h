#pragma once

#include <string_view>
#include <vector>

#include "io/json/json_value.h"

namespace objc3::io::json {

bool JsonSchemaTypeKeywordAllowsAny(const JsonValue &schema_type);
std::vector<std::string_view> JsonSchemaTypeKeywordNames(
    const JsonValue &schema_type);

}  // namespace objc3::io::json
